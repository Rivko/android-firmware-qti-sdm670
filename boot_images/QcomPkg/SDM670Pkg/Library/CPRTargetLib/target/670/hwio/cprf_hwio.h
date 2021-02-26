#ifndef __CPRF_HWIO_H__
#define __CPRF_HWIO_H__
/*
===========================================================================
*/
/**
  @file cprf_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r93]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_CPRF_CPRF

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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/CPRTargetLib/target/670/hwio/cprf_hwio.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"
#include "msmhwio.h"


/*----------------------------------------------------------------------------
 * MODULE: RPMH_CPRF_CPRF
 *--------------------------------------------------------------------------*/

#define RPMH_CPRF_CPRF_REG_BASE                                                      (AOSS_BASE      + 0x01200000)
#define RPMH_CPRF_CPRF_REG_BASE_PHYS                                                 (AOSS_BASE_PHYS + 0x01200000)
#define RPMH_CPRF_CPRF_REG_BASE_OFFS                                                 0x01200000

#define HWIO_RPMH_CPRF_HW_VERSION_ADDR                                               (RPMH_CPRF_CPRF_REG_BASE      + 0x00000000)
#define HWIO_RPMH_CPRF_HW_VERSION_PHYS                                               (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00000000)
#define HWIO_RPMH_CPRF_HW_VERSION_OFFS                                               (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00000000)
#define HWIO_RPMH_CPRF_HW_VERSION_RMSK                                               0xffffffff
#define HWIO_RPMH_CPRF_HW_VERSION_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_HW_VERSION_ADDR, HWIO_RPMH_CPRF_HW_VERSION_RMSK)
#define HWIO_RPMH_CPRF_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_HW_VERSION_ADDR, m)
#define HWIO_RPMH_CPRF_HW_VERSION_MAJOR_BMSK                                         0xf0000000
#define HWIO_RPMH_CPRF_HW_VERSION_MAJOR_SHFT                                               0x1c
#define HWIO_RPMH_CPRF_HW_VERSION_MINOR_BMSK                                          0xfff0000
#define HWIO_RPMH_CPRF_HW_VERSION_MINOR_SHFT                                               0x10
#define HWIO_RPMH_CPRF_HW_VERSION_STEP_BMSK                                              0xffff
#define HWIO_RPMH_CPRF_HW_VERSION_STEP_SHFT                                                 0x0

#define HWIO_RPMH_CPRF_AOSS_SETTINGS_ADDR                                            (RPMH_CPRF_CPRF_REG_BASE      + 0x00000004)
#define HWIO_RPMH_CPRF_AOSS_SETTINGS_PHYS                                            (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00000004)
#define HWIO_RPMH_CPRF_AOSS_SETTINGS_OFFS                                            (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00000004)
#define HWIO_RPMH_CPRF_AOSS_SETTINGS_RMSK                                                0xffff
#define HWIO_RPMH_CPRF_AOSS_SETTINGS_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_AOSS_SETTINGS_ADDR, HWIO_RPMH_CPRF_AOSS_SETTINGS_RMSK)
#define HWIO_RPMH_CPRF_AOSS_SETTINGS_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_AOSS_SETTINGS_ADDR, m)
#define HWIO_RPMH_CPRF_AOSS_SETTINGS_OUT(v)      \
        out_dword(HWIO_RPMH_CPRF_AOSS_SETTINGS_ADDR,v)
#define HWIO_RPMH_CPRF_AOSS_SETTINGS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_AOSS_SETTINGS_ADDR,m,v,HWIO_RPMH_CPRF_AOSS_SETTINGS_IN)
#define HWIO_RPMH_CPRF_AOSS_SETTINGS_SID_BMSK                                            0xe000
#define HWIO_RPMH_CPRF_AOSS_SETTINGS_SID_SHFT                                               0xd
#define HWIO_RPMH_CPRF_AOSS_SETTINGS_DRVID_BMSK                                          0x1f00
#define HWIO_RPMH_CPRF_AOSS_SETTINGS_DRVID_SHFT                                             0x8
#define HWIO_RPMH_CPRF_AOSS_SETTINGS_ADDRESS_BMSK                                          0xff
#define HWIO_RPMH_CPRF_AOSS_SETTINGS_ADDRESS_SHFT                                           0x0

#define HWIO_RPMH_CPRF_INTERRUPT_STATUS_ADDR                                         (RPMH_CPRF_CPRF_REG_BASE      + 0x00000008)
#define HWIO_RPMH_CPRF_INTERRUPT_STATUS_PHYS                                         (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00000008)
#define HWIO_RPMH_CPRF_INTERRUPT_STATUS_OFFS                                         (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00000008)
#define HWIO_RPMH_CPRF_INTERRUPT_STATUS_RMSK                                             0x3fff
#define HWIO_RPMH_CPRF_INTERRUPT_STATUS_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_INTERRUPT_STATUS_ADDR, HWIO_RPMH_CPRF_INTERRUPT_STATUS_RMSK)
#define HWIO_RPMH_CPRF_INTERRUPT_STATUS_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_INTERRUPT_STATUS_ADDR, m)
#define HWIO_RPMH_CPRF_INTERRUPT_STATUS_CPRF_VSENSE_INTERRUPT_STATUS_BMSK                0x3f80
#define HWIO_RPMH_CPRF_INTERRUPT_STATUS_CPRF_VSENSE_INTERRUPT_STATUS_SHFT                   0x7
#define HWIO_RPMH_CPRF_INTERRUPT_STATUS_CPRF_INTERRUPT_STATUS_BMSK                         0x7f
#define HWIO_RPMH_CPRF_INTERRUPT_STATUS_CPRF_INTERRUPT_STATUS_SHFT                          0x0

#define HWIO_RPMH_CPRF_VRM_COMPLETION_WA_ADDR                                        (RPMH_CPRF_CPRF_REG_BASE      + 0x0000000c)
#define HWIO_RPMH_CPRF_VRM_COMPLETION_WA_PHYS                                        (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000000c)
#define HWIO_RPMH_CPRF_VRM_COMPLETION_WA_OFFS                                        (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000000c)
#define HWIO_RPMH_CPRF_VRM_COMPLETION_WA_RMSK                                           0x1ffff
#define HWIO_RPMH_CPRF_VRM_COMPLETION_WA_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_VRM_COMPLETION_WA_ADDR, HWIO_RPMH_CPRF_VRM_COMPLETION_WA_RMSK)
#define HWIO_RPMH_CPRF_VRM_COMPLETION_WA_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_VRM_COMPLETION_WA_ADDR, m)
#define HWIO_RPMH_CPRF_VRM_COMPLETION_WA_OUT(v)      \
        out_dword(HWIO_RPMH_CPRF_VRM_COMPLETION_WA_ADDR,v)
#define HWIO_RPMH_CPRF_VRM_COMPLETION_WA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_VRM_COMPLETION_WA_ADDR,m,v,HWIO_RPMH_CPRF_VRM_COMPLETION_WA_IN)
#define HWIO_RPMH_CPRF_VRM_COMPLETION_WA_SW_COMPLETION_BMSK                             0x1fffe
#define HWIO_RPMH_CPRF_VRM_COMPLETION_WA_SW_COMPLETION_SHFT                                 0x1
#define HWIO_RPMH_CPRF_VRM_COMPLETION_WA_COMPLETION_WA_EN_BMSK                              0x1
#define HWIO_RPMH_CPRF_VRM_COMPLETION_WA_COMPLETION_WA_EN_SHFT                              0x0

#define HWIO_RPMH_CPRF_VRM_BRCST_WA_ADDR                                             (RPMH_CPRF_CPRF_REG_BASE      + 0x00000010)
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_PHYS                                             (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00000010)
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_OFFS                                             (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00000010)
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_RMSK                                              0xfffffff
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_VRM_BRCST_WA_ADDR, HWIO_RPMH_CPRF_VRM_BRCST_WA_RMSK)
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_VRM_BRCST_WA_ADDR, m)
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_OUT(v)      \
        out_dword(HWIO_RPMH_CPRF_VRM_BRCST_WA_ADDR,v)
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_VRM_BRCST_WA_ADDR,m,v,HWIO_RPMH_CPRF_VRM_BRCST_WA_IN)
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_SW_BRCST_VALID_BMSK                               0x8000000
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_SW_BRCST_VALID_SHFT                                    0x1b
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_SW_BRCST_CHANGE_DATA_BMSK                         0x7ffc000
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_SW_BRCST_CHANGE_DATA_SHFT                               0xe
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_SW_BRCST_TASK_TYPE_BMSK                              0x3e00
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_SW_BRCST_TASK_TYPE_SHFT                                 0x9
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_SW_BRCST_RESOURCE_ID_BMSK                             0x1fe
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_SW_BRCST_RESOURCE_ID_SHFT                               0x1
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_BRCST_WA_EN_BMSK                                        0x1
#define HWIO_RPMH_CPRF_VRM_BRCST_WA_BRCST_WA_EN_SHFT                                        0x0

#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_ADDR                                         (RPMH_CPRF_CPRF_REG_BASE      + 0x00000014)
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_PHYS                                         (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00000014)
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_OFFS                                         (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00000014)
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_RMSK                                         0xffffffff
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_ADDR, HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_RMSK)
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_ADDR, m)
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_OUT(v)      \
        out_dword(HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_ADDR,v)
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_ADDR,m,v,HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_IN)
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_DEBUG_SEL_BMSK                               0xfffffffe
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_DEBUG_SEL_SHFT                                      0x1
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_SELECT_CTRL_BMSK                                    0x1
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_0_SELECT_CTRL_SHFT                                    0x0

#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_ADDR                                         (RPMH_CPRF_CPRF_REG_BASE      + 0x00000018)
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_PHYS                                         (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00000018)
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_OFFS                                         (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00000018)
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_RMSK                                         0xffffffff
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_ADDR, HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_RMSK)
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_ADDR, m)
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_OUT(v)      \
        out_dword(HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_ADDR,v)
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_ADDR,m,v,HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_IN)
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_DEBUG_SEL_BMSK                               0xffffffff
#define HWIO_RPMH_CPRF_DEBUG_BUS_CTRL_1_DEBUG_SEL_SHFT                                      0x0

#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_0_ADDR                                       (RPMH_CPRF_CPRF_REG_BASE      + 0x0000001c)
#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_0_PHYS                                       (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000001c)
#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_0_OFFS                                       (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000001c)
#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_0_RMSK                                       0xffffffff
#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_0_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_0_ADDR, HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_0_RMSK)
#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_0_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_0_ADDR, m)
#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_0_DEBUG_BUS_BMSK                             0xffffffff
#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_0_DEBUG_BUS_SHFT                                    0x0

#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_1_ADDR                                       (RPMH_CPRF_CPRF_REG_BASE      + 0x00000020)
#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_1_PHYS                                       (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00000020)
#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_1_OFFS                                       (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00000020)
#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_1_RMSK                                       0xffffffff
#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_1_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_1_ADDR, HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_1_RMSK)
#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_1_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_1_ADDR, m)
#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_1_DEBUG_BUS_BMSK                             0xffffffff
#define HWIO_RPMH_CPRF_DEBUG_BUS_OUTPUT_1_DEBUG_BUS_SHFT                                    0x0

#define HWIO_RPMH_CPRF_CXO_ON_SETTINGS_ADDR                                          (RPMH_CPRF_CPRF_REG_BASE      + 0x00000024)
#define HWIO_RPMH_CPRF_CXO_ON_SETTINGS_PHYS                                          (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00000024)
#define HWIO_RPMH_CPRF_CXO_ON_SETTINGS_OFFS                                          (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00000024)
#define HWIO_RPMH_CPRF_CXO_ON_SETTINGS_RMSK                                                0x1f
#define HWIO_RPMH_CPRF_CXO_ON_SETTINGS_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_CXO_ON_SETTINGS_ADDR, HWIO_RPMH_CPRF_CXO_ON_SETTINGS_RMSK)
#define HWIO_RPMH_CPRF_CXO_ON_SETTINGS_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_CXO_ON_SETTINGS_ADDR, m)
#define HWIO_RPMH_CPRF_CXO_ON_SETTINGS_OUT(v)      \
        out_dword(HWIO_RPMH_CPRF_CXO_ON_SETTINGS_ADDR,v)
#define HWIO_RPMH_CPRF_CXO_ON_SETTINGS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CXO_ON_SETTINGS_ADDR,m,v,HWIO_RPMH_CPRF_CXO_ON_SETTINGS_IN)
#define HWIO_RPMH_CPRF_CXO_ON_SETTINGS_OL_ON_BMSK                                          0x1f
#define HWIO_RPMH_CPRF_CXO_ON_SETTINGS_OL_ON_SHFT                                           0x0

#define HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_ADDR                                         (RPMH_CPRF_CPRF_REG_BASE      + 0x00000028)
#define HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_PHYS                                         (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00000028)
#define HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_OFFS                                         (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00000028)
#define HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_RMSK                                                0x1
#define HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_ADDR, HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_RMSK)
#define HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_ADDR, m)
#define HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_OUT(v)      \
        out_dword(HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_ADDR,v)
#define HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_ADDR,m,v,HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_IN)
#define HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_SW_RESET_BMSK                                       0x1
#define HWIO_RPMH_CPRF_CLOCK_MGMT_RESET_SW_RESET_SHFT                                       0x0

#define HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_ADDR                                        (RPMH_CPRF_CPRF_REG_BASE      + 0x0000002c)
#define HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_PHYS                                        (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000002c)
#define HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_OFFS                                        (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000002c)
#define HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_RMSK                                               0x3
#define HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_ADDR, HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_RMSK)
#define HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_ADDR, m)
#define HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_OUT(v)      \
        out_dword(HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_ADDR,v)
#define HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_ADDR,m,v,HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_IN)
#define HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_CXO_OFF_POST_BOOT_BMSK                             0x2
#define HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_CXO_OFF_POST_BOOT_SHFT                             0x1
#define HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_EN_BMSK                                            0x1
#define HWIO_RPMH_CPRF_CLOCK_MGMT_ENABLE_EN_SHFT                                            0x0

#define HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_ADDR                                     (RPMH_CPRF_CPRF_REG_BASE      + 0x00000030)
#define HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_PHYS                                     (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00000030)
#define HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_OFFS                                     (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00000030)
#define HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_RMSK                                            0x7
#define HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_ADDR, HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_RMSK)
#define HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_ADDR, m)
#define HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_OUT(v)      \
        out_dword(HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_ADDR,v)
#define HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_ADDR,m,v,HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_IN)
#define HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_CHANGE_DATA_BMSK                                0x4
#define HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_CHANGE_DATA_SHFT                                0x2
#define HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_TASK_TYPE_BMSK                                  0x2
#define HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_TASK_TYPE_SHFT                                  0x1
#define HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_RESOURCE_ID_BMSK                                0x1
#define HWIO_RPMH_CPRF_VRM_BRCST_ENDIANNESS_RESOURCE_ID_SHFT                                0x0

#define HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_ADDR                                        (RPMH_CPRF_CPRF_REG_BASE      + 0x00000034)
#define HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_PHYS                                        (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00000034)
#define HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_OFFS                                        (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00000034)
#define HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_RMSK                                               0x3
#define HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_ADDR, HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_RMSK)
#define HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_ADDR, m)
#define HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_OUT(v)      \
        out_dword(HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_ADDR,v)
#define HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_ADDR,m,v,HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_IN)
#define HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_ENDIANNESS_BMSK                                    0x2
#define HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_ENDIANNESS_SHFT                                    0x1
#define HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_SWAP_BMSK                                          0x1
#define HWIO_RPMH_CPRF_ARC_CXO_BIT_ORDER_SWAP_SHFT                                          0x0

#define HWIO_RPMH_CPRF_MISC_RESET_ADDR                                               (RPMH_CPRF_CPRF_REG_BASE      + 0x00000038)
#define HWIO_RPMH_CPRF_MISC_RESET_PHYS                                               (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00000038)
#define HWIO_RPMH_CPRF_MISC_RESET_OFFS                                               (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00000038)
#define HWIO_RPMH_CPRF_MISC_RESET_RMSK                                                      0x1
#define HWIO_RPMH_CPRF_MISC_RESET_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_MISC_RESET_ADDR, HWIO_RPMH_CPRF_MISC_RESET_RMSK)
#define HWIO_RPMH_CPRF_MISC_RESET_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_MISC_RESET_ADDR, m)
#define HWIO_RPMH_CPRF_MISC_RESET_OUT(v)      \
        out_dword(HWIO_RPMH_CPRF_MISC_RESET_ADDR,v)
#define HWIO_RPMH_CPRF_MISC_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_MISC_RESET_ADDR,m,v,HWIO_RPMH_CPRF_MISC_RESET_IN)
#define HWIO_RPMH_CPRF_MISC_RESET_SW_RESET_BMSK                                             0x1
#define HWIO_RPMH_CPRF_MISC_RESET_SW_RESET_SHFT                                             0x0

#define HWIO_RPMH_CPRF_CPRFm_RESET_ADDR(m)                                           (RPMH_CPRF_CPRF_REG_BASE      + 0x00001000 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_RESET_PHYS(m)                                           (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001000 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_RESET_OFFS(m)                                           (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001000 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_RESET_RMSK                                                     0x1
#define HWIO_RPMH_CPRF_CPRFm_RESET_MAXm                                                       6
#define HWIO_RPMH_CPRF_CPRFm_RESET_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_RESET_ADDR(m), HWIO_RPMH_CPRF_CPRFm_RESET_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_RESET_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_RESET_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_RESET_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_RESET_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_RESET_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_RESET_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_RESET_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_RESET_SW_RESET_BMSK                                            0x1
#define HWIO_RPMH_CPRF_CPRFm_RESET_SW_RESET_SHFT                                            0x0

#define HWIO_RPMH_CPRF_CPRFm_CTRL_ADDR(m)                                            (RPMH_CPRF_CPRF_REG_BASE      + 0x00001004 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_CTRL_PHYS(m)                                            (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001004 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_CTRL_OFFS(m)                                            (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001004 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_CTRL_RMSK                                                     0xff
#define HWIO_RPMH_CPRF_CPRFm_CTRL_MAXm                                                        6
#define HWIO_RPMH_CPRF_CPRFm_CTRL_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_CTRL_ADDR(m), HWIO_RPMH_CPRF_CPRFm_CTRL_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_CTRL_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_CTRL_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_CTRL_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_CTRL_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_CTRL_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_CTRL_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_CTRL_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_CTRL_NOT_VOTE_EQUAL_VOLTAGE_BMSK                              0x80
#define HWIO_RPMH_CPRF_CPRFm_CTRL_NOT_VOTE_EQUAL_VOLTAGE_SHFT                               0x7
#define HWIO_RPMH_CPRF_CPRFm_CTRL_RE_ENABLE_STARVATION_BMSK                                0x40
#define HWIO_RPMH_CPRF_CPRFm_CTRL_RE_ENABLE_STARVATION_SHFT                                 0x6
#define HWIO_RPMH_CPRF_CPRFm_CTRL_COMPLETION_REQ_CPRC_BMSK                                 0x20
#define HWIO_RPMH_CPRF_CPRFm_CTRL_COMPLETION_REQ_CPRC_SHFT                                  0x5
#define HWIO_RPMH_CPRF_CPRFm_CTRL_COMPLETION_REQ_DCVS_BMSK                                 0x10
#define HWIO_RPMH_CPRF_CPRFm_CTRL_COMPLETION_REQ_DCVS_SHFT                                  0x4
#define HWIO_RPMH_CPRF_CPRFm_CTRL_PR_HANDSHAKE_EN_BMSK                                      0x8
#define HWIO_RPMH_CPRF_CPRFm_CTRL_PR_HANDSHAKE_EN_SHFT                                      0x3
#define HWIO_RPMH_CPRF_CPRFm_CTRL_CTRL_INTERACTION_EN_BMSK                                  0x4
#define HWIO_RPMH_CPRF_CPRFm_CTRL_CTRL_INTERACTION_EN_SHFT                                  0x2
#define HWIO_RPMH_CPRF_CPRFm_CTRL_CLOCK_MGMT_EN_BMSK                                        0x2
#define HWIO_RPMH_CPRF_CPRFm_CTRL_CLOCK_MGMT_EN_SHFT                                        0x1
#define HWIO_RPMH_CPRF_CPRFm_CTRL_VOLTAGE_VOTE_EN_BMSK                                      0x1
#define HWIO_RPMH_CPRF_CPRFm_CTRL_VOLTAGE_VOTE_EN_SHFT                                      0x0

#define HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_ADDR(m)                                    (RPMH_CPRF_CPRF_REG_BASE      + 0x00001008 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_PHYS(m)                                    (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001008 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_OFFS(m)                                    (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001008 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_RMSK                                       0x1fffffff
#define HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_MAXm                                                6
#define HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_ADDR(m), HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_VOLTAGE_ADDRESS_OFFSET_BMSK                0x1fffe000
#define HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_VOLTAGE_ADDRESS_OFFSET_SHFT                       0xd
#define HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_TASK_TYPE_BMSK                                 0x1f00
#define HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_TASK_TYPE_SHFT                                    0x8
#define HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_RESOURCE_ID_BMSK                                 0xff
#define HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_RESOURCE_ID_SHFT                                  0x0

#define HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_ADDR(m,n)                              (RPMH_CPRF_CPRF_REG_BASE      + 0x0000100c + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_PHYS(m,n)                              (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000100c + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_OFFS(m,n)                              (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000100c + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_RMSK                                   0x1fff1fff
#define HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_MAXm                                            6
#define HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_MAXn                                           15
#define HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_INI2(m,n)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_ADDR(m,n), HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_ADDR(m,n), mask)
#define HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_OUTI2(m,n,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_ADDR(m,n),val)
#define HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_OUTMI2(m,n,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_ADDR(m,n),mask,val,HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_INI2(m,n))
#define HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_VMAX_BMSK                              0x1fff0000
#define HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_VMAX_SHFT                                    0x10
#define HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_VMIN_BMSK                                  0x1fff
#define HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_VMIN_SHFT                                     0x0

#define HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_ADDR(m,n)                                (RPMH_CPRF_CPRF_REG_BASE      + 0x0000108c + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_PHYS(m,n)                                (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000108c + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_OFFS(m,n)                                (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000108c + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_RMSK                                         0x1fff
#define HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_MAXm                                              6
#define HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_MAXn                                             15
#define HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_INI2(m,n)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_ADDR(m,n), HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_ADDR(m,n), mask)
#define HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_OUTI2(m,n,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_ADDR(m,n),val)
#define HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_OUTMI2(m,n,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_ADDR(m,n),mask,val,HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_INI2(m,n))
#define HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_VSAFE_BMSK                                   0x1fff
#define HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_VSAFE_SHFT                                      0x0

#define HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_ADDR(m)                                      (RPMH_CPRF_CPRF_REG_BASE      + 0x00001110 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_PHYS(m)                                      (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001110 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_OFFS(m)                                      (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001110 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_RMSK                                         0xffffffff
#define HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_MAXm                                                  6
#define HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_ADDR(m), HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_COUNT_BMSK                                   0xfffffffe
#define HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_COUNT_SHFT                                          0x1
#define HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_ENABLE_BMSK                                         0x1
#define HWIO_RPMH_CPRF_CPRFm_DELAY_CPRC_ENABLE_SHFT                                         0x0

#define HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_ADDR(m)                                (RPMH_CPRF_CPRF_REG_BASE      + 0x00001114 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_PHYS(m)                                (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001114 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_OFFS(m)                                (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001114 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_RMSK                                         0x1f
#define HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_MAXm                                            6
#define HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_ADDR(m), HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_INITIAL_OL_BMSK                              0x1f
#define HWIO_RPMH_CPRF_CPRFm_INITIAL_SETTINGS_INITIAL_OL_SHFT                               0x0

#define HWIO_RPMH_CPRF_CPRFm_ENABLE_ADDR(m)                                          (RPMH_CPRF_CPRF_REG_BASE      + 0x00001118 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_ENABLE_PHYS(m)                                          (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001118 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_ENABLE_OFFS(m)                                          (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001118 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_ENABLE_RMSK                                                    0x1
#define HWIO_RPMH_CPRF_CPRFm_ENABLE_MAXm                                                      6
#define HWIO_RPMH_CPRF_CPRFm_ENABLE_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_ENABLE_ADDR(m), HWIO_RPMH_CPRF_CPRFm_ENABLE_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_ENABLE_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_ENABLE_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_ENABLE_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_ENABLE_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_ENABLE_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_ENABLE_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_ENABLE_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_ENABLE_EN_BMSK                                                 0x1
#define HWIO_RPMH_CPRF_CPRFm_ENABLE_EN_SHFT                                                 0x0

#define HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_ADDR(m)                                  (RPMH_CPRF_CPRF_REG_BASE      + 0x0000111c + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_PHYS(m)                                  (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000111c + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_OFFS(m)                                  (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000111c + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_RMSK                                            0x1
#define HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_MAXm                                              6
#define HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_ADDR(m), HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_FORCE_INITIAL_0_BMSK                            0x1
#define HWIO_RPMH_CPRF_CPRFm_FORCE_SETTINGS_FORCE_INITIAL_0_SHFT                            0x0

#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_ADDR(m,n)                          (RPMH_CPRF_CPRF_REG_BASE      + 0x00001120 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_PHYS(m,n)                          (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001120 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_OFFS(m,n)                          (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001120 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_RMSK                                     0xff
#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_MAXm                                        6
#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_MAXn                                       15
#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_INI2(m,n)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_ADDR(m,n), HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_ADDR(m,n), mask)
#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_OUTI2(m,n,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_ADDR(m,n),val)
#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_OUTMI2(m,n,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_ADDR(m,n),mask,val,HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_INI2(m,n))
#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_CONT_MID_COUNTER_BMSK                    0x80
#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_CONT_MID_COUNTER_SHFT                     0x7
#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_MID_COUNT_BMSK                           0x78
#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_MID_COUNT_SHFT                            0x3
#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_MODE_SEL_BMSK                             0x7
#define HWIO_RPMH_CPRF_CPRFm_RECORDING_SETTINGS_n_MODE_SEL_SHFT                             0x0

#define HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_ADDR(m,n)                           (RPMH_CPRF_CPRF_REG_BASE      + 0x000011a0 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_PHYS(m,n)                           (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x000011a0 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_OFFS(m,n)                           (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x000011a0 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_RMSK                                      0xff
#define HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_MAXm                                         6
#define HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_MAXn                                        15
#define HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_INI2(m,n)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_ADDR(m,n), HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_ADDR(m,n), mask)
#define HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_OUTI2(m,n,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_ADDR(m,n),val)
#define HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_OUTMI2(m,n,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_ADDR(m,n),mask,val,HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_INI2(m,n))
#define HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_CAP_UP_BMSK                               0xf0
#define HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_CAP_UP_SHFT                                0x4
#define HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_CAP_DOWN_BMSK                              0xf
#define HWIO_RPMH_CPRF_CPRFm_CAP_STEP_SETTINGS_n_CAP_DOWN_SHFT                              0x0

#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_ADDR(m)                                (RPMH_CPRF_CPRF_REG_BASE      + 0x00001220 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_PHYS(m)                                (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001220 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_OFFS(m)                                (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001220 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_RMSK                                        0x7ff
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_MAXm                                            6
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_ADDR(m), HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_CPRC_SUCCESS_INT_BMSK                       0x400
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_CPRC_SUCCESS_INT_SHFT                         0xa
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_DCVS_SUCCESS_INT_BMSK                       0x200
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_DCVS_SUCCESS_INT_SHFT                         0x9
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_TIMEOUT_INT_BMSK                            0x100
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_TIMEOUT_INT_SHFT                              0x8
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_FLOOR_UPDATED_INT_BMSK                       0x80
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_FLOOR_UPDATED_INT_SHFT                        0x7
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_ERROR_VMIN_INT_BMSK                          0x40
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_ERROR_VMIN_INT_SHFT                           0x6
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_ERROR_VMAX_INT_BMSK                          0x20
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_ERROR_VMAX_INT_SHFT                           0x5
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_NEW_CPRC_RECOMM_INT_BMSK                     0x10
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_NEW_CPRC_RECOMM_INT_SHFT                      0x4
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_NEW_VOLTAGE_VOTE_INT_BMSK                     0x8
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_NEW_VOLTAGE_VOTE_INT_SHFT                     0x3
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_PR_OL_CHANGED_INT_BMSK                        0x4
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_PR_OL_CHANGED_INT_SHFT                        0x2
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_CXO_ENABLED_INT_BMSK                          0x2
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_CXO_ENABLED_INT_SHFT                          0x1
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_CXO_DISABLED_INT_BMSK                         0x1
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_STATUS_CXO_DISABLED_INT_SHFT                         0x0

#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_ADDR(m)                                   (RPMH_CPRF_CPRF_REG_BASE      + 0x00001220 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_PHYS(m)                                   (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001220 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_OFFS(m)                                   (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001220 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_RMSK                                           0x7ff
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_MAXm                                               6
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_CPRC_SUCCESS_ACK_BMSK                          0x400
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_CPRC_SUCCESS_ACK_SHFT                            0xa
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_DCVS_SUCCESS_ACK_BMSK                          0x200
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_DCVS_SUCCESS_ACK_SHFT                            0x9
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_TIMEOUT_ACK_BMSK                               0x100
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_TIMEOUT_ACK_SHFT                                 0x8
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_FLOOR_UPDATED_ACK_BMSK                          0x80
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_FLOOR_UPDATED_ACK_SHFT                           0x7
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_ERROR_VMIN_ACK_BMSK                             0x40
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_ERROR_VMIN_ACK_SHFT                              0x6
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_ERROR_VMAX_ACK_BMSK                             0x20
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_ERROR_VMAX_ACK_SHFT                              0x5
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_NEW_CPRC_RECOMM_ACK_BMSK                        0x10
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_NEW_CPRC_RECOMM_ACK_SHFT                         0x4
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_NEW_VOLTAGE_VOTE_ACK_BMSK                        0x8
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_NEW_VOLTAGE_VOTE_ACK_SHFT                        0x3
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_PR_OL_CHANGED_ACK_BMSK                           0x4
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_PR_OL_CHANGED_ACK_SHFT                           0x2
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_CXO_ENABLED_ACK_BMSK                             0x2
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_CXO_ENABLED_ACK_SHFT                             0x1
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_CXO_DISABLED_ACK_BMSK                            0x1
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_ACK_CXO_DISABLED_ACK_SHFT                            0x0

#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_ADDR(m)                                  (RPMH_CPRF_CPRF_REG_BASE      + 0x00001224 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_PHYS(m)                                  (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001224 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_OFFS(m)                                  (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001224 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_RMSK                                          0x7ff
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_MAXm                                              6
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_ADDR(m), HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_CPRC_SUCCESS_MASK_BMSK                        0x400
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_CPRC_SUCCESS_MASK_SHFT                          0xa
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_DCVS_SUCCESS_MASK_BMSK                        0x200
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_DCVS_SUCCESS_MASK_SHFT                          0x9
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_TIMEOUT_MASK_BMSK                             0x100
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_TIMEOUT_MASK_SHFT                               0x8
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_FLOOR_UPDATED_MASK_BMSK                        0x80
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_FLOOR_UPDATED_MASK_SHFT                         0x7
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_ERROR_VMIN_MASK_BMSK                           0x40
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_ERROR_VMIN_MASK_SHFT                            0x6
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_ERROR_VMAX_MASK_BMSK                           0x20
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_ERROR_VMAX_MASK_SHFT                            0x5
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_NEW_CPRC_RECOMM_MASK_BMSK                      0x10
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_NEW_CPRC_RECOMM_MASK_SHFT                       0x4
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_NEW_VOLTAGE_VOTE_MASK_BMSK                      0x8
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_NEW_VOLTAGE_VOTE_MASK_SHFT                      0x3
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_PR_OL_CHANGED_MASK_BMSK                         0x4
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_PR_OL_CHANGED_MASK_SHFT                         0x2
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_CXO_ENABLED_MASK_BMSK                           0x2
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_CXO_ENABLED_MASK_SHFT                           0x1
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_CXO_DISABLED_MASK_BMSK                          0x1
#define HWIO_RPMH_CPRF_CPRFm_INTERRUPT_MASK_CXO_DISABLED_MASK_SHFT                          0x0

#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_ADDR(m)                                       (RPMH_CPRF_CPRF_REG_BASE      + 0x00001228 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_PHYS(m)                                       (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001228 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_OFFS(m)                                       (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001228 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_RMSK                                              0x3fff
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_MAXm                                                   6
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_ADDR(m), HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_SW_PR_POST_REQ_BMSK                               0x2000
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_SW_PR_POST_REQ_SHFT                                  0xd
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_SW_PR_CHANGE_REQ_BMSK                             0x1000
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_SW_PR_CHANGE_REQ_SHFT                                0xc
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_SW_PR_PRE_REQ_BMSK                                 0x800
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_SW_PR_PRE_REQ_SHFT                                   0xb
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_SW_PR_DEST_OL_BMSK                                 0x7c0
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_SW_PR_DEST_OL_SHFT                                   0x6
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_SW_PR_CUR_OL_BMSK                                   0x3e
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_SW_PR_CUR_OL_SHFT                                    0x1
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_PR_ARC_WA_EN_BMSK                                    0x1
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_WA_PR_ARC_WA_EN_SHFT                                    0x0

#define HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_ADDR(m)                                      (RPMH_CPRF_CPRF_REG_BASE      + 0x0000122c + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_PHYS(m)                                      (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000122c + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_OFFS(m)                                      (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000122c + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_RMSK                                                0x7
#define HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_MAXm                                                  6
#define HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_ADDR(m), HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_CLOCK_CHANGED_BMSK                                  0x4
#define HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_CLOCK_CHANGED_SHFT                                  0x2
#define HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_SW_CLOCK_STATE_BMSK                                 0x2
#define HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_SW_CLOCK_STATE_SHFT                                 0x1
#define HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_CXO_ARC_WA_EN_BMSK                                  0x1
#define HWIO_RPMH_CPRF_CPRFm_ARC_CXO_WA_CXO_ARC_WA_EN_SHFT                                  0x0

#define HWIO_RPMH_CPRF_CPRFm_VALUES_ADDR(m)                                          (RPMH_CPRF_CPRF_REG_BASE      + 0x00001230 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_PHYS(m)                                          (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001230 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_OFFS(m)                                          (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001230 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_RMSK                                              0x3ff1fff
#define HWIO_RPMH_CPRF_CPRFm_VALUES_MAXm                                                      6
#define HWIO_RPMH_CPRF_CPRFm_VALUES_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VALUES_ADDR(m), HWIO_RPMH_CPRF_CPRFm_VALUES_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_VALUES_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VALUES_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_VALUES_ERROR_STEPS_BMSK                                  0x3e00000
#define HWIO_RPMH_CPRF_CPRFm_VALUES_ERROR_STEPS_SHFT                                       0x15
#define HWIO_RPMH_CPRF_CPRFm_VALUES_CUR_OL_BMSK                                        0x1f0000
#define HWIO_RPMH_CPRF_CPRFm_VALUES_CUR_OL_SHFT                                            0x10
#define HWIO_RPMH_CPRF_CPRFm_VALUES_TENT_VOL_BMSK                                        0x1fff
#define HWIO_RPMH_CPRF_CPRFm_VALUES_TENT_VOL_SHFT                                           0x0

#define HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_ADDR(m)                                   (RPMH_CPRF_CPRF_REG_BASE      + 0x00001234 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_PHYS(m)                                   (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001234 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_OFFS(m)                                   (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001234 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_RMSK                                           0x3ff
#define HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_MAXm                                               6
#define HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_ADDR(m), HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_TOLERANCE_BMSK                                 0x3e0
#define HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_TOLERANCE_SHFT                                   0x5
#define HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_STEP_BMSK                                       0x1f
#define HWIO_RPMH_CPRF_CPRFm_STEP_SETTINGS_STEP_SHFT                                        0x0

#define HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_ADDR(m)                                  (RPMH_CPRF_CPRF_REG_BASE      + 0x00001238 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_PHYS(m)                                  (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001238 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_OFFS(m)                                  (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001238 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_RMSK                                         0x1fff
#define HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_MAXm                                              6
#define HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_ADDR(m), HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_RUNNING_FLOOR_BMSK                           0x1fff
#define HWIO_RPMH_CPRF_CPRFm_FLOOR_SETTINGS_RUNNING_FLOOR_SHFT                              0x0

#define HWIO_RPMH_CPRF_CPRFm_TIMEOUT_ADDR(m)                                         (RPMH_CPRF_CPRF_REG_BASE      + 0x0000123c + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_TIMEOUT_PHYS(m)                                         (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000123c + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_TIMEOUT_OFFS(m)                                         (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000123c + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_TIMEOUT_RMSK                                            0xffffffff
#define HWIO_RPMH_CPRF_CPRFm_TIMEOUT_MAXm                                                     6
#define HWIO_RPMH_CPRF_CPRFm_TIMEOUT_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_TIMEOUT_ADDR(m), HWIO_RPMH_CPRF_CPRFm_TIMEOUT_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_TIMEOUT_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_TIMEOUT_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_TIMEOUT_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_TIMEOUT_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_TIMEOUT_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_TIMEOUT_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_TIMEOUT_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_TIMEOUT_COUNT_BMSK                                      0xfffffffe
#define HWIO_RPMH_CPRF_CPRFm_TIMEOUT_COUNT_SHFT                                             0x1
#define HWIO_RPMH_CPRF_CPRFm_TIMEOUT_ENABLE_BMSK                                            0x1
#define HWIO_RPMH_CPRF_CPRFm_TIMEOUT_ENABLE_SHFT                                            0x0

#define HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_ADDR(m)                                     (RPMH_CPRF_CPRF_REG_BASE      + 0x00001240 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_PHYS(m)                                     (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001240 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_OFFS(m)                                     (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001240 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_RMSK                                             0x3ff
#define HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_MAXm                                                 6
#define HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_ADDR(m), HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_OL_ON_VOTE_BMSK                                  0x3e0
#define HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_OL_ON_VOTE_SHFT                                    0x5
#define HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_OL_ON_BMSK                                        0x1f
#define HWIO_RPMH_CPRF_CPRFm_ON_SETTINGS_OL_ON_SHFT                                         0x0

#define HWIO_RPMH_CPRF_CPRFm_VALUES_1_ADDR(m)                                        (RPMH_CPRF_CPRF_REG_BASE      + 0x00001244 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_1_PHYS(m)                                        (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001244 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_1_OFFS(m)                                        (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001244 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_1_RMSK                                           0x1fff1fff
#define HWIO_RPMH_CPRF_CPRFm_VALUES_1_MAXm                                                    6
#define HWIO_RPMH_CPRF_CPRFm_VALUES_1_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VALUES_1_ADDR(m), HWIO_RPMH_CPRF_CPRFm_VALUES_1_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_VALUES_1_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VALUES_1_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_VALUES_1_CURRENT_VOL_BMSK                               0x1fff0000
#define HWIO_RPMH_CPRF_CPRFm_VALUES_1_CURRENT_VOL_SHFT                                     0x10
#define HWIO_RPMH_CPRF_CPRFm_VALUES_1_VOL_VOTE_BMSK                                      0x1fff
#define HWIO_RPMH_CPRF_CPRFm_VALUES_1_VOL_VOTE_SHFT                                         0x0

#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_ADDR(m)                                (RPMH_CPRF_CPRF_REG_BASE      + 0x00001248 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_PHYS(m)                                (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001248 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_OFFS(m)                                (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001248 + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_RMSK                                          0x3
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_MAXm                                            6
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_ADDR(m), HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_ENDIANNESS_BMSK                               0x2
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_ENDIANNESS_SHFT                               0x1
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_SWAP_BMSK                                     0x1
#define HWIO_RPMH_CPRF_CPRFm_ARC_PR_BIT_ORDER_SWAP_SHFT                                     0x0

#define HWIO_RPMH_CPRF_CPRFm_DELTA_n_ADDR(m,n)                                       (RPMH_CPRF_CPRF_REG_BASE      + 0x0000124c + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_DELTA_n_PHYS(m,n)                                       (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000124c + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_DELTA_n_OFFS(m,n)                                       (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000124c + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_DELTA_n_RMSK                                                 0x1ff
#define HWIO_RPMH_CPRF_CPRFm_DELTA_n_MAXm                                                     6
#define HWIO_RPMH_CPRF_CPRFm_DELTA_n_MAXn                                                    15
#define HWIO_RPMH_CPRF_CPRFm_DELTA_n_INI2(m,n)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_DELTA_n_ADDR(m,n), HWIO_RPMH_CPRF_CPRFm_DELTA_n_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_DELTA_n_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_DELTA_n_ADDR(m,n), mask)
#define HWIO_RPMH_CPRF_CPRFm_DELTA_n_OUTI2(m,n,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_DELTA_n_ADDR(m,n),val)
#define HWIO_RPMH_CPRF_CPRFm_DELTA_n_OUTMI2(m,n,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_DELTA_n_ADDR(m,n),mask,val,HWIO_RPMH_CPRF_CPRFm_DELTA_n_INI2(m,n))
#define HWIO_RPMH_CPRF_CPRFm_DELTA_n_MAX_BMSK                                             0x1fe
#define HWIO_RPMH_CPRF_CPRFm_DELTA_n_MAX_SHFT                                               0x1
#define HWIO_RPMH_CPRF_CPRFm_DELTA_n_EN_BMSK                                                0x1
#define HWIO_RPMH_CPRF_CPRFm_DELTA_n_EN_SHFT                                                0x0

#define HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_ADDR(m)                                  (RPMH_CPRF_CPRF_REG_BASE      + 0x000012cc + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_PHYS(m)                                  (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x000012cc + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_OFFS(m)                                  (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x000012cc + 0x1000 * (m))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_RMSK                                            0x3
#define HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_MAXm                                              6
#define HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_ADDR(m), HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_INI(m))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_RESET_UD_COUNT_BMSK                             0x2
#define HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_RESET_UD_COUNT_SHFT                             0x1
#define HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_RESET_OL_COUNT_BMSK                             0x1
#define HWIO_RPMH_CPRF_CPRFm_VALUES_CONTROL_RESET_OL_COUNT_SHFT                             0x0

#define HWIO_RPMH_CPRF_CPRFm_VALUES_2_n_ADDR(m,n)                                    (RPMH_CPRF_CPRF_REG_BASE      + 0x000012d0 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_2_n_PHYS(m,n)                                    (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x000012d0 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_2_n_OFFS(m,n)                                    (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x000012d0 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_2_n_RMSK                                         0x1fff1fff
#define HWIO_RPMH_CPRF_CPRFm_VALUES_2_n_MAXm                                                  6
#define HWIO_RPMH_CPRF_CPRFm_VALUES_2_n_MAXn                                                 15
#define HWIO_RPMH_CPRF_CPRFm_VALUES_2_n_INI2(m,n)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VALUES_2_n_ADDR(m,n), HWIO_RPMH_CPRF_CPRFm_VALUES_2_n_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_VALUES_2_n_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VALUES_2_n_ADDR(m,n), mask)
#define HWIO_RPMH_CPRF_CPRFm_VALUES_2_n_MAX_VOL_BMSK                                 0x1fff0000
#define HWIO_RPMH_CPRF_CPRFm_VALUES_2_n_MAX_VOL_SHFT                                       0x10
#define HWIO_RPMH_CPRF_CPRFm_VALUES_2_n_MIN_VOL_BMSK                                     0x1fff
#define HWIO_RPMH_CPRF_CPRFm_VALUES_2_n_MIN_VOL_SHFT                                        0x0

#define HWIO_RPMH_CPRF_CPRFm_VALUES_3_n_ADDR(m,n)                                    (RPMH_CPRF_CPRF_REG_BASE      + 0x00001350 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_3_n_PHYS(m,n)                                    (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001350 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_3_n_OFFS(m,n)                                    (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001350 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_3_n_RMSK                                             0x1fff
#define HWIO_RPMH_CPRF_CPRFm_VALUES_3_n_MAXm                                                  6
#define HWIO_RPMH_CPRF_CPRFm_VALUES_3_n_MAXn                                                 15
#define HWIO_RPMH_CPRF_CPRFm_VALUES_3_n_INI2(m,n)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VALUES_3_n_ADDR(m,n), HWIO_RPMH_CPRF_CPRFm_VALUES_3_n_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_VALUES_3_n_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VALUES_3_n_ADDR(m,n), mask)
#define HWIO_RPMH_CPRF_CPRFm_VALUES_3_n_LAST_VOL_BMSK                                    0x1fff
#define HWIO_RPMH_CPRF_CPRFm_VALUES_3_n_LAST_VOL_SHFT                                       0x0

#define HWIO_RPMH_CPRF_CPRFm_VALUES_4_n_ADDR(m,n)                                    (RPMH_CPRF_CPRF_REG_BASE      + 0x000013d0 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_4_n_PHYS(m,n)                                    (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x000013d0 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_4_n_OFFS(m,n)                                    (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x000013d0 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_4_n_RMSK                                               0xff
#define HWIO_RPMH_CPRF_CPRFm_VALUES_4_n_MAXm                                                  6
#define HWIO_RPMH_CPRF_CPRFm_VALUES_4_n_MAXn                                                 15
#define HWIO_RPMH_CPRF_CPRFm_VALUES_4_n_INI2(m,n)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VALUES_4_n_ADDR(m,n), HWIO_RPMH_CPRF_CPRFm_VALUES_4_n_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_VALUES_4_n_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VALUES_4_n_ADDR(m,n), mask)
#define HWIO_RPMH_CPRF_CPRFm_VALUES_4_n_OL_COUNT_BMSK                                      0xff
#define HWIO_RPMH_CPRF_CPRFm_VALUES_4_n_OL_COUNT_SHFT                                       0x0

#define HWIO_RPMH_CPRF_CPRFm_VALUES_5_n_ADDR(m,n)                                    (RPMH_CPRF_CPRF_REG_BASE      + 0x00001450 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_5_n_PHYS(m,n)                                    (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x00001450 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_5_n_OFFS(m,n)                                    (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x00001450 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_RPMH_CPRF_CPRFm_VALUES_5_n_RMSK                                           0xff00ff
#define HWIO_RPMH_CPRF_CPRFm_VALUES_5_n_MAXm                                                  6
#define HWIO_RPMH_CPRF_CPRFm_VALUES_5_n_MAXn                                                 15
#define HWIO_RPMH_CPRF_CPRFm_VALUES_5_n_INI2(m,n)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VALUES_5_n_ADDR(m,n), HWIO_RPMH_CPRF_CPRFm_VALUES_5_n_RMSK)
#define HWIO_RPMH_CPRF_CPRFm_VALUES_5_n_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRFm_VALUES_5_n_ADDR(m,n), mask)
#define HWIO_RPMH_CPRF_CPRFm_VALUES_5_n_DOWN_COUNT_BMSK                                0xff0000
#define HWIO_RPMH_CPRF_CPRFm_VALUES_5_n_DOWN_COUNT_SHFT                                    0x10
#define HWIO_RPMH_CPRF_CPRFm_VALUES_5_n_UP_COUNT_BMSK                                      0xff
#define HWIO_RPMH_CPRF_CPRFm_VALUES_5_n_UP_COUNT_SHFT                                       0x0

#define HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_ADDR(m)                                    (RPMH_CPRF_CPRF_REG_BASE      + 0x0000a000 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_PHYS(m)                                    (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000a000 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_OFFS(m)                                    (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000a000 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_RMSK                                              0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_MAXm                                                6
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_ADDR(m), HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_RMSK)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_INI(m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_SW_RESET_BMSK                                     0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_RESET_SW_RESET_SHFT                                     0x0

#define HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_ADDR(m)                                     (RPMH_CPRF_CPRF_REG_BASE      + 0x0000a004 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_PHYS(m)                                     (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000a004 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_OFFS(m)                                     (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000a004 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_RMSK                                               0x7
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_MAXm                                                 6
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_ADDR(m), HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_RMSK)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_INI(m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_PR_HANDSHAKE_EN_BMSK                               0x4
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_PR_HANDSHAKE_EN_SHFT                               0x2
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_CTRL_INTERACTION_EN_BMSK                           0x2
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_CTRL_INTERACTION_EN_SHFT                           0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_CLOCK_MGMT_EN_BMSK                                 0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_CTRL_CLOCK_MGMT_EN_SHFT                                 0x0

#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_ADDR(m)                         (RPMH_CPRF_CPRF_REG_BASE      + 0x0000a008 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_PHYS(m)                         (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000a008 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_OFFS(m)                         (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000a008 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_RMSK                                  0x1f
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_MAXm                                     6
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_ADDR(m), HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_RMSK)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_INI(m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_INITIAL_OL_BMSK                       0x1f
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INITIAL_SETTINGS_INITIAL_OL_SHFT                        0x0

#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_ADDR(m)                                   (RPMH_CPRF_CPRF_REG_BASE      + 0x0000a00c + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_PHYS(m)                                   (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000a00c + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_OFFS(m)                                   (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000a00c + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_RMSK                                             0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_MAXm                                               6
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_ADDR(m), HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_RMSK)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_INI(m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_EN_BMSK                                          0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ENABLE_EN_SHFT                                          0x0

#define HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_ADDR(m)                           (RPMH_CPRF_CPRF_REG_BASE      + 0x0000a010 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_PHYS(m)                           (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000a010 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_OFFS(m)                           (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000a010 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_RMSK                                     0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_MAXm                                       6
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_ADDR(m), HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_RMSK)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_INI(m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_FORCE_INITIAL_0_BMSK                     0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_FORCE_SETTINGS_FORCE_INITIAL_0_SHFT                     0x0

#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_ADDR(m)                         (RPMH_CPRF_CPRF_REG_BASE      + 0x0000a014 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_PHYS(m)                         (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000a014 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_OFFS(m)                         (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000a014 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_RMSK                                   0x7
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_MAXm                                     6
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_ADDR(m), HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_RMSK)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_PR_OL_CHANGED_INT_BMSK                 0x4
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_PR_OL_CHANGED_INT_SHFT                 0x2
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_CXO_ENABLED_INT_BMSK                   0x2
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_CXO_ENABLED_INT_SHFT                   0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_CXO_DISABLED_INT_BMSK                  0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_STATUS_CXO_DISABLED_INT_SHFT                  0x0

#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_ACK_ADDR(m)                            (RPMH_CPRF_CPRF_REG_BASE      + 0x0000a014 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_ACK_PHYS(m)                            (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000a014 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_ACK_OFFS(m)                            (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000a014 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_ACK_RMSK                                      0x7
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_ACK_MAXm                                        6
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_ACK_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_ACK_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_ACK_PR_OL_CHANGED_ACK_BMSK                    0x4
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_ACK_PR_OL_CHANGED_ACK_SHFT                    0x2
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_ACK_CXO_ENABLED_ACK_BMSK                      0x2
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_ACK_CXO_ENABLED_ACK_SHFT                      0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_ACK_CXO_DISABLED_ACK_BMSK                     0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_ACK_CXO_DISABLED_ACK_SHFT                     0x0

#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_ADDR(m)                           (RPMH_CPRF_CPRF_REG_BASE      + 0x0000a018 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_PHYS(m)                           (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000a018 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_OFFS(m)                           (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000a018 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_RMSK                                     0x7
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_MAXm                                       6
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_ADDR(m), HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_RMSK)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_INI(m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_PR_OL_CHANGED_MASK_BMSK                  0x4
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_PR_OL_CHANGED_MASK_SHFT                  0x2
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_CXO_ENABLED_MASK_BMSK                    0x2
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_CXO_ENABLED_MASK_SHFT                    0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_CXO_DISABLED_MASK_BMSK                   0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_INTERRUPT_MASK_CXO_DISABLED_MASK_SHFT                   0x0

#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_ADDR(m)                                (RPMH_CPRF_CPRF_REG_BASE      + 0x0000a01c + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_PHYS(m)                                (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000a01c + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_OFFS(m)                                (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000a01c + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_RMSK                                       0x3fff
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_MAXm                                            6
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_ADDR(m), HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_RMSK)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_INI(m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_SW_PR_POST_REQ_BMSK                        0x2000
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_SW_PR_POST_REQ_SHFT                           0xd
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_SW_PR_CHANGE_REQ_BMSK                      0x1000
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_SW_PR_CHANGE_REQ_SHFT                         0xc
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_SW_PR_PRE_REQ_BMSK                          0x800
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_SW_PR_PRE_REQ_SHFT                            0xb
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_SW_PR_DEST_OL_BMSK                          0x7c0
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_SW_PR_DEST_OL_SHFT                            0x6
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_SW_PR_CUR_OL_BMSK                            0x3e
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_SW_PR_CUR_OL_SHFT                             0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_PR_ARC_WA_EN_BMSK                             0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_WA_PR_ARC_WA_EN_SHFT                             0x0

#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_ADDR(m)                               (RPMH_CPRF_CPRF_REG_BASE      + 0x0000a020 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_PHYS(m)                               (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000a020 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_OFFS(m)                               (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000a020 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_RMSK                                         0x7
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_MAXm                                           6
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_ADDR(m), HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_RMSK)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_INI(m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_CLOCK_CHANGED_BMSK                           0x4
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_CLOCK_CHANGED_SHFT                           0x2
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_SW_CLOCK_STATE_BMSK                          0x2
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_SW_CLOCK_STATE_SHFT                          0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_CXO_ARC_WA_EN_BMSK                           0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_CXO_WA_CXO_ARC_WA_EN_SHFT                           0x0

#define HWIO_RPMH_CPRF_CPRF_VSENSEm_VALUES_ADDR(m)                                   (RPMH_CPRF_CPRF_REG_BASE      + 0x0000a024 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_VALUES_PHYS(m)                                   (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000a024 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_VALUES_OFFS(m)                                   (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000a024 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_VALUES_RMSK                                            0x1f
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_VALUES_MAXm                                               6
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_VALUES_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_VALUES_ADDR(m), HWIO_RPMH_CPRF_CPRF_VSENSEm_VALUES_RMSK)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_VALUES_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_VALUES_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_VALUES_CUR_OL_BMSK                                     0x1f
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_VALUES_CUR_OL_SHFT                                      0x0

#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_ADDR(m)                              (RPMH_CPRF_CPRF_REG_BASE      + 0x0000a028 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_PHYS(m)                              (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000a028 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_OFFS(m)                              (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000a028 + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_RMSK                                       0x1f
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_MAXm                                          6
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_ADDR(m), HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_RMSK)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_INI(m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_OL_ON_BMSK                                 0x1f
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ON_SETTINGS_OL_ON_SHFT                                  0x0

#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_ADDR(m)                         (RPMH_CPRF_CPRF_REG_BASE      + 0x0000a02c + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_PHYS(m)                         (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000a02c + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_OFFS(m)                         (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000a02c + 0x200 * (m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_RMSK                                   0x3
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_MAXm                                     6
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_INI(m)        \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_ADDR(m), HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_RMSK)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_ADDR(m), mask)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_OUTI(m,val)    \
        out_dword(HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_ADDR(m),val)
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_ADDR(m),mask,val,HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_INI(m))
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_ENDIANNESS_BMSK                        0x2
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_ENDIANNESS_SHFT                        0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_SWAP_BMSK                              0x1
#define HWIO_RPMH_CPRF_CPRF_VSENSEm_ARC_PR_BIT_ORDER_SWAP_SHFT                              0x0

#define HWIO_RPMH_CPRF_VRM_MESSAGE_ADDR                                              (RPMH_CPRF_CPRF_REG_BASE      + 0x0000f000)
#define HWIO_RPMH_CPRF_VRM_MESSAGE_PHYS                                              (RPMH_CPRF_CPRF_REG_BASE_PHYS + 0x0000f000)
#define HWIO_RPMH_CPRF_VRM_MESSAGE_OFFS                                              (RPMH_CPRF_CPRF_REG_BASE_OFFS + 0x0000f000)
#define HWIO_RPMH_CPRF_VRM_MESSAGE_RMSK                                                  0xffff
#define HWIO_RPMH_CPRF_VRM_MESSAGE_IN          \
        in_dword_masked(HWIO_RPMH_CPRF_VRM_MESSAGE_ADDR, HWIO_RPMH_CPRF_VRM_MESSAGE_RMSK)
#define HWIO_RPMH_CPRF_VRM_MESSAGE_INM(m)      \
        in_dword_masked(HWIO_RPMH_CPRF_VRM_MESSAGE_ADDR, m)
#define HWIO_RPMH_CPRF_VRM_MESSAGE_OUT(v)      \
        out_dword(HWIO_RPMH_CPRF_VRM_MESSAGE_ADDR,v)
#define HWIO_RPMH_CPRF_VRM_MESSAGE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_CPRF_VRM_MESSAGE_ADDR,m,v,HWIO_RPMH_CPRF_VRM_MESSAGE_IN)
#define HWIO_RPMH_CPRF_VRM_MESSAGE_COMPLETION_BMSK                                       0xffff
#define HWIO_RPMH_CPRF_VRM_MESSAGE_COMPLETION_SHFT                                          0x0


#endif /* __CPRF_HWIO_H__ */
