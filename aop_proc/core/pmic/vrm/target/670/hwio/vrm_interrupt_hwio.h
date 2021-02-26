#ifndef __VRM_INTERRUPT_HWIO_H__
#define __VRM_INTERRUPT_HWIO_H__
/*
===========================================================================
*/
/**
  @file vrm_interrupt_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) v2 [napali_v2.0_p3q2r35_MTO_with_AOSS_USB_PHY_ECOs_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_VRM_VRM_INTERRUPT


  Generation parameters: 
  { u'filename': u'vrm_interrupt_hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'RPMH_VRM_VRM_INTERRUPT']}
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/target/670/hwio/vrm_interrupt_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: RPMH_VRM_VRM_INTERRUPT
 *--------------------------------------------------------------------------*/

#define RPMH_VRM_VRM_INTERRUPT_REG_BASE                                                                       (AOSS_BASE      + 0x009f1000)

#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_0_CSR_ADDR                                                       (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000000)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_0_CSR_RMSK                                                       0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_0_CSR_ADDR, HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_0_CSR_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_0_CSR_VOLTAGE_DEP_IRQ_STATUS_0_BMSK                              0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_0_CSR_VOLTAGE_DEP_IRQ_STATUS_0_SHFT                                     0x0

#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_1_CSR_ADDR                                                       (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000004)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_1_CSR_RMSK                                                           0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_1_CSR_ADDR, HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_1_CSR_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_1_CSR_VOLTAGE_DEP_IRQ_STATUS_1_BMSK                                  0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_1_CSR_VOLTAGE_DEP_IRQ_STATUS_1_SHFT                                     0x0

#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_0_CSR_ADDR                                                        (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000018)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_0_CSR_RMSK                                                        0xffffffff
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_0_CSR_ADDR, HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_0_CSR_RMSK)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_0_CSR_ENABLE_DEP_IRQ_STATUS_0_BMSK                                0xffffffff
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_0_CSR_ENABLE_DEP_IRQ_STATUS_0_SHFT                                       0x0

#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_1_CSR_ADDR                                                        (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x0000001c)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_1_CSR_RMSK                                                         0x7fc3fff
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_1_CSR_ADDR, HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_1_CSR_RMSK)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_1_CSR_XO_DEP_IRQ_STATUS_BMSK                                       0x4000000
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_1_CSR_XO_DEP_IRQ_STATUS_SHFT                                            0x1a
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_1_CSR_XOB_DEP_IRQ_STATUS_BMSK                                      0x3fc0000
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_1_CSR_XOB_DEP_IRQ_STATUS_SHFT                                           0x12
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_1_CSR_REG_DEP_IRQ_STATUS_BMSK                                         0x3fff
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_STATUS_1_CSR_REG_DEP_IRQ_STATUS_SHFT                                            0x0

#define HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_0_CSR_ADDR                                                          (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000038)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_0_CSR_RMSK                                                          0xffffffff
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_0_CSR_ADDR, HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_0_CSR_RMSK)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_0_CSR_MODE_DEP_IRQ_STATUS_0_BMSK                                    0xffffffff
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_0_CSR_MODE_DEP_IRQ_STATUS_0_SHFT                                           0x0

#define HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_1_CSR_ADDR                                                          (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x0000003c)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_1_CSR_RMSK                                                              0x3fff
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_1_CSR_ADDR, HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_1_CSR_RMSK)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_1_CSR_MODE_DEP_IRQ_STATUS_1_BMSK                                        0x3fff
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_STATUS_1_CSR_MODE_DEP_IRQ_STATUS_1_SHFT                                           0x0

#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_0_CSR_ADDR                                                      (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000050)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_0_CSR_RMSK                                                      0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_0_CSR_ADDR, HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_0_CSR_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_0_CSR_HEADROOM_DEP_IRQ_STATUS_0_BMSK                            0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_0_CSR_HEADROOM_DEP_IRQ_STATUS_0_SHFT                                   0x0

#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_1_CSR_ADDR                                                      (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000054)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_1_CSR_RMSK                                                          0x3fff
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_1_CSR_ADDR, HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_1_CSR_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_1_CSR_HEADROOM_DEP_IRQ_STATUS_1_BMSK                                0x3fff
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_1_CSR_HEADROOM_DEP_IRQ_STATUS_1_SHFT                                   0x0

#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_0_CSR_ADDR                                                    (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000108)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_0_CSR_RMSK                                                    0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_0_CSR_ADDR, HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_0_CSR_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_0_CSR_VOLTAGE_CHANGE_IRQ_STATUS_0_BMSK                        0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_0_CSR_VOLTAGE_CHANGE_IRQ_STATUS_0_SHFT                               0x0

#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_1_CSR_ADDR                                                    (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x0000010c)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_1_CSR_RMSK                                                        0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_1_CSR_ADDR, HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_1_CSR_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_1_CSR_VOLTAGE_CHANGE_IRQ_STATUS_1_BMSK                            0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_1_CSR_VOLTAGE_CHANGE_IRQ_STATUS_1_SHFT                               0x0

#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_0_CSR_ADDR                                                     (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000120)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_0_CSR_RMSK                                                     0xffffffff
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_0_CSR_ADDR, HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_0_CSR_RMSK)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_0_CSR_ENABLE_CHANGE_IRQ_STATUS_0_BMSK                          0xffffffff
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_0_CSR_ENABLE_CHANGE_IRQ_STATUS_0_SHFT                                 0x0

#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_1_CSR_ADDR                                                     (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000124)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_1_CSR_RMSK                                                      0x7fc3fff
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_1_CSR_ADDR, HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_1_CSR_RMSK)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_1_CSR_XO_CHANGE_IRQ_STATUS_BMSK                                 0x4000000
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_1_CSR_XO_CHANGE_IRQ_STATUS_SHFT                                      0x1a
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_1_CSR_XOB_CHANGE_IRQ_STATUS_BMSK                                0x3fc0000
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_1_CSR_XOB_CHANGE_IRQ_STATUS_SHFT                                     0x12
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_1_CSR_REG_CHANGE_IRQ_STATUS_BMSK                                   0x3fff
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_1_CSR_REG_CHANGE_IRQ_STATUS_SHFT                                      0x0

#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_0_CSR_ADDR                                                       (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000140)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_0_CSR_RMSK                                                       0xffffffff
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_0_CSR_ADDR, HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_0_CSR_RMSK)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_0_CSR_MODE_CHANGE_IRQ_STATUS_0_BMSK                              0xffffffff
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_0_CSR_MODE_CHANGE_IRQ_STATUS_0_SHFT                                     0x0

#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_1_CSR_ADDR                                                       (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000144)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_1_CSR_RMSK                                                           0x3fff
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_1_CSR_ADDR, HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_1_CSR_RMSK)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_1_CSR_MODE_CHANGE_IRQ_STATUS_1_BMSK                                  0x3fff
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_STATUS_1_CSR_MODE_CHANGE_IRQ_STATUS_1_SHFT                                     0x0

#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_0_CSR_ADDR                                                   (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000158)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_0_CSR_RMSK                                                   0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_0_CSR_ADDR, HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_0_CSR_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_0_CSR_HEADROOM_CHANGE_IRQ_STATUS_0_BMSK                      0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_0_CSR_HEADROOM_CHANGE_IRQ_STATUS_0_SHFT                             0x0

#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_1_CSR_ADDR                                                   (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x0000015c)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_1_CSR_RMSK                                                       0x3fff
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_1_CSR_ADDR, HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_1_CSR_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_1_CSR_HEADROOM_CHANGE_IRQ_STATUS_1_BMSK                          0x3fff
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_1_CSR_HEADROOM_CHANGE_IRQ_STATUS_1_SHFT                             0x0

#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_0_CSR_ADDR                                                   (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000170)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_0_CSR_RMSK                                                   0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_0_CSR_ADDR, HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_0_CSR_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_0_CSR_VOLTAGE_SETTLED_IRQ_STATUS_0_BMSK                      0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_0_CSR_VOLTAGE_SETTLED_IRQ_STATUS_0_SHFT                             0x0

#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_1_CSR_ADDR                                                   (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000174)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_1_CSR_RMSK                                                       0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_1_CSR_ADDR, HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_1_CSR_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_1_CSR_VOLTAGE_SETTLED_IRQ_STATUS_1_BMSK                          0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_1_CSR_VOLTAGE_SETTLED_IRQ_STATUS_1_SHFT                             0x0

#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_0_CSR_ADDR                                                    (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000188)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_0_CSR_RMSK                                                    0xffffffff
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_0_CSR_ADDR, HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_0_CSR_RMSK)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_0_CSR_ENABLE_SETTLED_IRQ_STATUS_0_BMSK                        0xffffffff
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_0_CSR_ENABLE_SETTLED_IRQ_STATUS_0_SHFT                               0x0

#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_1_CSR_ADDR                                                    (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x0000018c)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_1_CSR_RMSK                                                     0x7fc3fff
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_1_CSR_ADDR, HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_1_CSR_RMSK)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_1_CSR_XO_SETTLED_IRQ_STATUS_BMSK                               0x4000000
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_1_CSR_XO_SETTLED_IRQ_STATUS_SHFT                                    0x1a
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_1_CSR_XOB_SETTLED_IRQ_STATUS_BMSK                              0x3fc0000
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_1_CSR_XOB_SETTLED_IRQ_STATUS_SHFT                                   0x12
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_1_CSR_REG_SETTLED_IRQ_STATUS_BMSK                                 0x3fff
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_1_CSR_REG_SETTLED_IRQ_STATUS_SHFT                                    0x0

#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_0_CSR_ADDR                                                      (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000001a0)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_0_CSR_RMSK                                                      0xffffffff
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_0_CSR_ADDR, HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_0_CSR_RMSK)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_0_CSR_MODE_SETTLED_IRQ_STATUS_0_BMSK                            0xffffffff
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_0_CSR_MODE_SETTLED_IRQ_STATUS_0_SHFT                                   0x0

#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_1_CSR_ADDR                                                      (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000001a4)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_1_CSR_RMSK                                                          0x3fff
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_1_CSR_ADDR, HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_1_CSR_RMSK)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_1_CSR_MODE_SETTLED_IRQ_STATUS_1_BMSK                                0x3fff
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_STATUS_1_CSR_MODE_SETTLED_IRQ_STATUS_1_SHFT                                   0x0

#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_0_CSR_ADDR                                                  (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000001b8)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_0_CSR_RMSK                                                  0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_0_CSR_ADDR, HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_0_CSR_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_0_CSR_HEADROOM_SETTLED_IRQ_STATUS_0_BMSK                    0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_0_CSR_HEADROOM_SETTLED_IRQ_STATUS_0_SHFT                           0x0

#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_1_CSR_ADDR                                                  (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000001bc)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_1_CSR_RMSK                                                      0x3fff
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_1_CSR_ADDR, HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_1_CSR_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_1_CSR_HEADROOM_SETTLED_IRQ_STATUS_1_BMSK                        0x3fff
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_1_CSR_HEADROOM_SETTLED_IRQ_STATUS_1_SHFT                           0x0

#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_0_CSR_ADDR                                                   (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000270)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_0_CSR_RMSK                                                   0xffffffff
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_0_CSR_ADDR, HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_0_CSR_RMSK)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_0_CSR_VREG_READY_WDOG_IRQ_STATUS_0_BMSK                      0xffffffff
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_0_CSR_VREG_READY_WDOG_IRQ_STATUS_0_SHFT                             0x0

#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_1_CSR_ADDR                                                   (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000274)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_1_CSR_RMSK                                                       0x3fff
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_1_CSR_ADDR, HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_1_CSR_RMSK)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_1_CSR_VREG_READY_WDOG_IRQ_STATUS_1_BMSK                          0x3fff
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_1_CSR_VREG_READY_WDOG_IRQ_STATUS_1_SHFT                             0x0

#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR_ADDR                                                       (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000002c0)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR_RMSK                                                       0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR_ADDR, HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR_ADDR,m,v,HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR_IN)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR_VOLTAGE_DEP_IRQ_ENABLE_0_BMSK                              0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR_VOLTAGE_DEP_IRQ_ENABLE_0_SHFT                                     0x0

#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR_ADDR                                                       (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000002c4)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR_RMSK                                                           0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR_ADDR, HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR_ADDR,m,v,HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR_IN)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR_VOLTAGE_DEP_IRQ_ENABLE_1_BMSK                                  0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR_VOLTAGE_DEP_IRQ_ENABLE_1_SHFT                                     0x0

#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR_ADDR                                                        (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000002d8)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR_RMSK                                                        0xffffffff
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR_ADDR, HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR_RMSK)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR_ADDR,m,v,HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR_IN)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR_ENABLE_DEP_IRQ_ENABLE_0_BMSK                                0xffffffff
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR_ENABLE_DEP_IRQ_ENABLE_0_SHFT                                       0x0

#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_ADDR                                                        (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000002dc)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_RMSK                                                         0x7fc3fff
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_ADDR, HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_RMSK)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_ADDR,m,v,HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_IN)
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_XO_DEP_IRQ_ENABLE_BMSK                                       0x4000000
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_XO_DEP_IRQ_ENABLE_SHFT                                            0x1a
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_XOB_DEP_IRQ_ENABLE_BMSK                                      0x3fc0000
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_XOB_DEP_IRQ_ENABLE_SHFT                                           0x12
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_REG_DEP_IRQ_ENABLE_BMSK                                         0x3fff
#define HWIO_RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR_REG_DEP_IRQ_ENABLE_SHFT                                            0x0

#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR_ADDR                                                          (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000002f8)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR_RMSK                                                          0xffffffff
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR_ADDR, HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR_RMSK)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR_ADDR,m,v,HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR_IN)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR_MODE_DEP_IRQ_ENABLE_0_BMSK                                    0xffffffff
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR_MODE_DEP_IRQ_ENABLE_0_SHFT                                           0x0

#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR_ADDR                                                          (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000002fc)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR_RMSK                                                              0x3fff
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR_ADDR, HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR_RMSK)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR_ADDR,m,v,HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR_IN)
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR_MODE_DEP_IRQ_ENABLE_1_BMSK                                        0x3fff
#define HWIO_RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR_MODE_DEP_IRQ_ENABLE_1_SHFT                                           0x0

#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR_ADDR                                                      (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000310)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR_RMSK                                                      0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR_ADDR, HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR_ADDR,m,v,HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR_IN)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR_HEADROOM_DEP_IRQ_ENABLE_0_BMSK                            0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR_HEADROOM_DEP_IRQ_ENABLE_0_SHFT                                   0x0

#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR_ADDR                                                      (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000314)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR_RMSK                                                          0x3fff
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR_ADDR, HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR_ADDR,m,v,HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR_IN)
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR_HEADROOM_DEP_IRQ_ENABLE_1_BMSK                                0x3fff
#define HWIO_RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR_HEADROOM_DEP_IRQ_ENABLE_1_SHFT                                   0x0

#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR_ADDR                                                    (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000003c8)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR_RMSK                                                    0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR_ADDR, HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR_ADDR,m,v,HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR_IN)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR_VOLTAGE_CHANGE_IRQ_ENABLE_0_BMSK                        0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR_VOLTAGE_CHANGE_IRQ_ENABLE_0_SHFT                               0x0

#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR_ADDR                                                    (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000003cc)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR_RMSK                                                        0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR_ADDR, HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR_ADDR,m,v,HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR_IN)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR_VOLTAGE_CHANGE_IRQ_ENABLE_1_BMSK                            0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR_VOLTAGE_CHANGE_IRQ_ENABLE_1_SHFT                               0x0

#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR_ADDR                                                     (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000003e0)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR_RMSK                                                     0xffffffff
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR_ADDR, HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR_RMSK)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR_ADDR,m,v,HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR_IN)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR_ENABLE_CHANGE_IRQ_ENABLE_0_BMSK                          0xffffffff
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR_ENABLE_CHANGE_IRQ_ENABLE_0_SHFT                                 0x0

#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_ADDR                                                     (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000003e4)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_RMSK                                                      0x7fc3fff
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_ADDR, HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_RMSK)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_ADDR,m,v,HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_IN)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_XO_CHANGE_IRQ_ENABLE_BMSK                                 0x4000000
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_XO_CHANGE_IRQ_ENABLE_SHFT                                      0x1a
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_XOB_CHANGE_IRQ_ENABLE_BMSK                                0x3fc0000
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_XOB_CHANGE_IRQ_ENABLE_SHFT                                     0x12
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_REG_CHANGE_IRQ_ENABLE_BMSK                                   0x3fff
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR_REG_CHANGE_IRQ_ENABLE_SHFT                                      0x0

#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR_ADDR                                                       (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000400)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR_RMSK                                                       0xffffffff
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR_ADDR, HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR_RMSK)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR_ADDR,m,v,HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR_IN)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR_MODE_CHANGE_IRQ_ENABLE_0_BMSK                              0xffffffff
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR_MODE_CHANGE_IRQ_ENABLE_0_SHFT                                     0x0

#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR_ADDR                                                       (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000404)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR_RMSK                                                           0x3fff
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR_ADDR, HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR_RMSK)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR_ADDR,m,v,HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR_IN)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR_MODE_CHANGE_IRQ_ENABLE_1_BMSK                                  0x3fff
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR_MODE_CHANGE_IRQ_ENABLE_1_SHFT                                     0x0

#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR_ADDR                                                   (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000418)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR_RMSK                                                   0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR_ADDR, HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR_ADDR,m,v,HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR_IN)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR_HEADROOM_CHANGE_IRQ_ENABLE_0_BMSK                      0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR_HEADROOM_CHANGE_IRQ_ENABLE_0_SHFT                             0x0

#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR_ADDR                                                   (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x0000041c)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR_RMSK                                                       0x3fff
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR_ADDR, HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR_ADDR,m,v,HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR_IN)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR_HEADROOM_CHANGE_IRQ_ENABLE_1_BMSK                          0x3fff
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR_HEADROOM_CHANGE_IRQ_ENABLE_1_SHFT                             0x0

#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR_ADDR                                                   (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000004d0)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR_RMSK                                                   0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR_ADDR, HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR_ADDR,m,v,HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR_IN)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR_VOLTAGE_SETTLED_IRQ_ENABLE_0_BMSK                      0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR_VOLTAGE_SETTLED_IRQ_ENABLE_0_SHFT                             0x0

#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR_ADDR                                                   (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000004d4)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR_RMSK                                                       0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR_ADDR, HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR_ADDR,m,v,HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR_IN)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR_VOLTAGE_SETTLED_IRQ_ENABLE_1_BMSK                          0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR_VOLTAGE_SETTLED_IRQ_ENABLE_1_SHFT                             0x0

#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR_ADDR                                                    (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000004e8)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR_RMSK                                                    0xffffffff
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR_ADDR, HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR_RMSK)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR_ADDR,m,v,HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR_IN)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR_ENABLE_SETTLED_IRQ_ENABLE_0_BMSK                        0xffffffff
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR_ENABLE_SETTLED_IRQ_ENABLE_0_SHFT                               0x0

#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_ADDR                                                    (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000004ec)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_RMSK                                                     0x7fc3fff
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_ADDR, HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_RMSK)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_ADDR,m,v,HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_IN)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_XO_SETTLED_IRQ_ENABLE_BMSK                               0x4000000
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_XO_SETTLED_IRQ_ENABLE_SHFT                                    0x1a
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_XOB_SETTLED_IRQ_ENABLE_BMSK                              0x3fc0000
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_XOB_SETTLED_IRQ_ENABLE_SHFT                                   0x12
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_REG_SETTLED_IRQ_ENABLE_BMSK                                 0x3fff
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR_REG_SETTLED_IRQ_ENABLE_SHFT                                    0x0

#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR_ADDR                                                      (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000508)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR_RMSK                                                      0xffffffff
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR_ADDR, HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR_RMSK)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR_ADDR,m,v,HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR_IN)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR_MODE_SETTLED_IRQ_ENABLE_0_BMSK                            0xffffffff
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR_MODE_SETTLED_IRQ_ENABLE_0_SHFT                                   0x0

#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR_ADDR                                                      (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x0000050c)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR_RMSK                                                          0x3fff
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR_ADDR, HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR_RMSK)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR_ADDR,m,v,HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR_IN)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR_MODE_SETTLED_IRQ_ENABLE_1_BMSK                                0x3fff
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR_MODE_SETTLED_IRQ_ENABLE_1_SHFT                                   0x0

#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR_ADDR                                                  (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000520)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR_RMSK                                                  0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR_ADDR, HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR_ADDR,m,v,HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR_IN)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR_HEADROOM_SETTLED_IRQ_ENABLE_0_BMSK                    0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR_HEADROOM_SETTLED_IRQ_ENABLE_0_SHFT                           0x0

#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR_ADDR                                                  (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000524)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR_RMSK                                                      0x3fff
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR_ADDR, HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR_ADDR,m,v,HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR_IN)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR_HEADROOM_SETTLED_IRQ_ENABLE_1_BMSK                        0x3fff
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR_HEADROOM_SETTLED_IRQ_ENABLE_1_SHFT                           0x0

#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_0_CSR_ADDR                                                   (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000538)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_0_CSR_RMSK                                                   0xffffffff
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_0_CSR_ADDR, HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_0_CSR_RMSK)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_0_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_0_CSR_ADDR,m,v,HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_0_CSR_IN)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_0_CSR_VREG_READY_WDOG_IRQ_ENABLE_0_BMSK                      0xffffffff
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_0_CSR_VREG_READY_WDOG_IRQ_ENABLE_0_SHFT                             0x0

#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_1_CSR_ADDR                                                   (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x0000053c)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_1_CSR_RMSK                                                       0x3fff
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_1_CSR_ADDR, HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_1_CSR_RMSK)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_1_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_1_CSR_ADDR,m,v,HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_1_CSR_IN)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_1_CSR_VREG_READY_WDOG_IRQ_ENABLE_1_BMSK                          0x3fff
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_1_CSR_VREG_READY_WDOG_IRQ_ENABLE_1_SHFT                             0x0

#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_CLEAR_0_CSR_ADDR                                                     (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000588)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_CLEAR_0_CSR_RMSK                                                     0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_CLEAR_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_CLEAR_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_CLEAR_0_CSR_VOLTAGE_CHANGE_IRQ_CLEAR_0_BMSK                          0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_CLEAR_0_CSR_VOLTAGE_CHANGE_IRQ_CLEAR_0_SHFT                                 0x0

#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_CLEAR_1_CSR_ADDR                                                     (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x0000058c)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_CLEAR_1_CSR_RMSK                                                         0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_CLEAR_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_CLEAR_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_CLEAR_1_CSR_VOLTAGE_CHANGE_IRQ_CLEAR_1_BMSK                              0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_CHANGE_IRQ_CLEAR_1_CSR_VOLTAGE_CHANGE_IRQ_CLEAR_1_SHFT                                 0x0

#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_0_CSR_ADDR                                                      (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000005a0)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_0_CSR_RMSK                                                      0xffffffff
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_0_CSR_ENABLE_CHANGE_IRQ_CLEAR_0_BMSK                            0xffffffff
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_0_CSR_ENABLE_CHANGE_IRQ_CLEAR_0_SHFT                                   0x0

#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_1_CSR_ADDR                                                      (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000005a4)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_1_CSR_RMSK                                                       0x7fc3fff
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_1_CSR_XO_CHANGE_IRQ_CLEAR_BMSK                                   0x4000000
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_1_CSR_XO_CHANGE_IRQ_CLEAR_SHFT                                        0x1a
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_1_CSR_XOB_CHANGE_IRQ_CLEAR_BMSK                                  0x3fc0000
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_1_CSR_XOB_CHANGE_IRQ_CLEAR_SHFT                                       0x12
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_1_CSR_REG_CHANGE_IRQ_CLEAR_BMSK                                     0x3fff
#define HWIO_RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_1_CSR_REG_CHANGE_IRQ_CLEAR_SHFT                                        0x0

#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_CLEAR_0_CSR_ADDR                                                        (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000005c0)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_CLEAR_0_CSR_RMSK                                                        0xffffffff
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_CLEAR_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_MODE_CHANGE_IRQ_CLEAR_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_CLEAR_0_CSR_MODE_CHANGE_IRQ_CLEAR_0_BMSK                                0xffffffff
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_CLEAR_0_CSR_MODE_CHANGE_IRQ_CLEAR_0_SHFT                                       0x0

#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_CLEAR_1_CSR_ADDR                                                        (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000005c4)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_CLEAR_1_CSR_RMSK                                                            0x3fff
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_CLEAR_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_MODE_CHANGE_IRQ_CLEAR_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_CLEAR_1_CSR_MODE_CHANGE_IRQ_CLEAR_1_BMSK                                    0x3fff
#define HWIO_RPMH_VRM_MODE_CHANGE_IRQ_CLEAR_1_CSR_MODE_CHANGE_IRQ_CLEAR_1_SHFT                                       0x0

#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_CLEAR_0_CSR_ADDR                                                    (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000005d8)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_CLEAR_0_CSR_RMSK                                                    0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_CLEAR_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_CLEAR_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_CLEAR_0_CSR_HEADROOM_CHANGE_IRQ_CLEAR_0_BMSK                        0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_CLEAR_0_CSR_HEADROOM_CHANGE_IRQ_CLEAR_0_SHFT                               0x0

#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_CLEAR_1_CSR_ADDR                                                    (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000005dc)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_CLEAR_1_CSR_RMSK                                                        0x3fff
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_CLEAR_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_CLEAR_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_CLEAR_1_CSR_HEADROOM_CHANGE_IRQ_CLEAR_1_BMSK                            0x3fff
#define HWIO_RPMH_VRM_HEADROOM_CHANGE_IRQ_CLEAR_1_CSR_HEADROOM_CHANGE_IRQ_CLEAR_1_SHFT                               0x0

#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_CLEAR_0_CSR_ADDR                                                    (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000690)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_CLEAR_0_CSR_RMSK                                                    0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_CLEAR_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_CLEAR_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_CLEAR_0_CSR_VOLTAGE_SETTLED_IRQ_CLEAR_0_BMSK                        0xffffffff
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_CLEAR_0_CSR_VOLTAGE_SETTLED_IRQ_CLEAR_0_SHFT                               0x0

#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_CLEAR_1_CSR_ADDR                                                    (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000694)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_CLEAR_1_CSR_RMSK                                                        0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_CLEAR_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_CLEAR_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_CLEAR_1_CSR_VOLTAGE_SETTLED_IRQ_CLEAR_1_BMSK                            0x3fff
#define HWIO_RPMH_VRM_VOLTAGE_SETTLED_IRQ_CLEAR_1_CSR_VOLTAGE_SETTLED_IRQ_CLEAR_1_SHFT                               0x0

#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_0_CSR_ADDR                                                     (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000006a8)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_0_CSR_RMSK                                                     0xffffffff
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_0_CSR_ENABLE_SETTLED_IRQ_CLEAR_0_BMSK                          0xffffffff
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_0_CSR_ENABLE_SETTLED_IRQ_CLEAR_0_SHFT                                 0x0

#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_1_CSR_ADDR                                                     (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000006ac)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_1_CSR_RMSK                                                      0x7fc3fff
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_1_CSR_XO_SETTLED_IRQ_CLEAR_BMSK                                 0x4000000
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_1_CSR_XO_SETTLED_IRQ_CLEAR_SHFT                                      0x1a
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_1_CSR_XOB_SETTLED_IRQ_CLEAR_BMSK                                0x3fc0000
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_1_CSR_XOB_SETTLED_IRQ_CLEAR_SHFT                                     0x12
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_1_CSR_REG_SETTLED_IRQ_CLEAR_BMSK                                   0x3fff
#define HWIO_RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_1_CSR_REG_SETTLED_IRQ_CLEAR_SHFT                                      0x0

#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_CLEAR_0_CSR_ADDR                                                       (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000006c8)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_CLEAR_0_CSR_RMSK                                                       0xffffffff
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_CLEAR_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_MODE_SETTLED_IRQ_CLEAR_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_CLEAR_0_CSR_MODE_SETTLED_IRQ_CLEAR_0_BMSK                              0xffffffff
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_CLEAR_0_CSR_MODE_SETTLED_IRQ_CLEAR_0_SHFT                                     0x0

#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_CLEAR_1_CSR_ADDR                                                       (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000006cc)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_CLEAR_1_CSR_RMSK                                                           0x3fff
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_CLEAR_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_MODE_SETTLED_IRQ_CLEAR_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_CLEAR_1_CSR_MODE_SETTLED_IRQ_CLEAR_1_BMSK                                  0x3fff
#define HWIO_RPMH_VRM_MODE_SETTLED_IRQ_CLEAR_1_CSR_MODE_SETTLED_IRQ_CLEAR_1_SHFT                                     0x0

#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_CLEAR_0_CSR_ADDR                                                   (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000006e0)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_CLEAR_0_CSR_RMSK                                                   0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_CLEAR_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_CLEAR_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_CLEAR_0_CSR_HEADROOM_SETTLED_IRQ_CLEAR_0_BMSK                      0xffffffff
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_CLEAR_0_CSR_HEADROOM_SETTLED_IRQ_CLEAR_0_SHFT                             0x0

#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_CLEAR_1_CSR_ADDR                                                   (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000006e4)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_CLEAR_1_CSR_RMSK                                                       0x3fff
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_CLEAR_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_CLEAR_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_CLEAR_1_CSR_HEADROOM_SETTLED_IRQ_CLEAR_1_BMSK                          0x3fff
#define HWIO_RPMH_VRM_HEADROOM_SETTLED_IRQ_CLEAR_1_CSR_HEADROOM_SETTLED_IRQ_CLEAR_1_SHFT                             0x0

#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_CLEAR_0_CSR_ADDR                                                    (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000798)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_CLEAR_0_CSR_RMSK                                                    0xffffffff
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_CLEAR_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_CLEAR_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_CLEAR_0_CSR_VREG_READY_WDOG_IRQ_CLEAR_0_BMSK                        0xffffffff
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_CLEAR_0_CSR_VREG_READY_WDOG_IRQ_CLEAR_0_SHFT                               0x0

#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_CLEAR_1_CSR_ADDR                                                    (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x0000079c)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_CLEAR_1_CSR_RMSK                                                        0x3fff
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_CLEAR_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_CLEAR_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_CLEAR_1_CSR_VREG_READY_WDOG_IRQ_CLEAR_1_BMSK                            0x3fff
#define HWIO_RPMH_VRM_VREG_READY_WDOG_IRQ_CLEAR_1_CSR_VREG_READY_WDOG_IRQ_CLEAR_1_SHFT                               0x0

#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_0_CSR_ADDR                                             (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000007b4)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_0_CSR_RMSK                                             0xffffffff
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_0_CSR_ADDR, HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_0_CSR_RMSK)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_0_CSR_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_0_BMSK          0xffffffff
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_0_CSR_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_0_SHFT                 0x0

#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_1_CSR_ADDR                                             (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000007b8)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_1_CSR_RMSK                                                 0x3fff
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_1_CSR_ADDR, HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_1_CSR_RMSK)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_1_CSR_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_1_BMSK              0x3fff
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_1_CSR_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_1_SHFT                 0x0

#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR_ADDR                                             (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000007c0)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR_RMSK                                             0xffffffff
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR_ADDR, HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR_RMSK)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR_ADDR,m,v,HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR_IN)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_BMSK          0xffffffff
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_SHFT                 0x0

#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR_ADDR                                             (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000007c4)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR_RMSK                                                 0x3fff
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR_ADDR, HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR_RMSK)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR_ADDR, m)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR_ADDR,m,v,HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR_IN)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_BMSK              0x3fff
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_SHFT                 0x0

#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_0_CSR_ADDR                                              (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000008c0)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_0_CSR_RMSK                                              0xffffffff
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_0_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_0_CSR_ADDR,v)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_0_CSR_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_0_BMSK            0xffffffff
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_0_CSR_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_0_SHFT                   0x0

#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_1_CSR_ADDR                                              (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000008c4)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_1_CSR_RMSK                                                  0x3fff
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_1_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_1_CSR_ADDR,v)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_1_CSR_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_1_BMSK                0x3fff
#define HWIO_RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_1_CSR_MAX_VOLTAGE_VIOLATION_IRQ_CLEAR_1_SHFT                   0x0

#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_STATUS_CSR_ADDR                                                        (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000008cc)
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_STATUS_CSR_RMSK                                                            0x3fff
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_STATUS_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_STATUS_CSR_ADDR, HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_STATUS_CSR_RMSK)
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_STATUS_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_STATUS_CSR_ADDR, m)
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_STATUS_CSR_CTT_OVERFLOW_IRQ_STATUS_BMSK                                    0x3fff
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_STATUS_CSR_CTT_OVERFLOW_IRQ_STATUS_SHFT                                       0x0

#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_ENABLE_CSR_ADDR                                                        (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000008d0)
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_ENABLE_CSR_RMSK                                                            0x3fff
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_ENABLE_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_ENABLE_CSR_ADDR, HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_ENABLE_CSR_RMSK)
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_ENABLE_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_ENABLE_CSR_ADDR, m)
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_ENABLE_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_ENABLE_CSR_ADDR,v)
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_ENABLE_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_ENABLE_CSR_ADDR,m,v,HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_ENABLE_CSR_IN)
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_ENABLE_CSR_CTT_OVERFLOW_IRQ_ENABLE_BMSK                                    0x3fff
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_ENABLE_CSR_CTT_OVERFLOW_IRQ_ENABLE_SHFT                                       0x0

#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_CLEAR_CSR_ADDR                                                         (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000008d4)
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_CLEAR_CSR_RMSK                                                             0x3fff
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_CLEAR_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_CLEAR_CSR_ADDR,v)
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_CLEAR_CSR_CTT_OVERFLOW_IRQ_CLEAR_BMSK                                      0x3fff
#define HWIO_RPMH_VRM_CTT_OVERFLOW_IRQ_CLEAR_CSR_CTT_OVERFLOW_IRQ_CLEAR_SHFT                                         0x0

#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_STATUS_CSR_ADDR                                                      (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000008d8)
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_STATUS_CSR_RMSK                                                             0x1
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_STATUS_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_STATUS_CSR_ADDR, HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_STATUS_CSR_RMSK)
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_STATUS_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_STATUS_CSR_ADDR, m)
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_STATUS_CSR_S_PORT_FAILURE_IRQ_STATUS_BMSK                                   0x1
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_STATUS_CSR_S_PORT_FAILURE_IRQ_STATUS_SHFT                                   0x0

#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_ENABLE_CSR_ADDR                                                      (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000008dc)
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_ENABLE_CSR_RMSK                                                             0x1
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_ENABLE_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_ENABLE_CSR_ADDR, HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_ENABLE_CSR_RMSK)
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_ENABLE_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_ENABLE_CSR_ADDR, m)
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_ENABLE_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_ENABLE_CSR_ADDR,v)
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_ENABLE_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_ENABLE_CSR_ADDR,m,v,HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_ENABLE_CSR_IN)
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_ENABLE_CSR_S_PORT_FAILURE_IRQ_ENABLE_BMSK                                   0x1
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_ENABLE_CSR_S_PORT_FAILURE_IRQ_ENABLE_SHFT                                   0x0

#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_CLEAR_CSR_ADDR                                                       (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000008e0)
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_CLEAR_CSR_RMSK                                                              0x1
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_CLEAR_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_CLEAR_CSR_ADDR,v)
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_CLEAR_CSR_S_PORT_FAILURE_IRQ_CLEAR_BMSK                                     0x1
#define HWIO_RPMH_VRM_S_PORT_FAILURE_IRQ_CLEAR_CSR_S_PORT_FAILURE_IRQ_CLEAR_SHFT                                     0x0

#define HWIO_RPMH_VRM_SW_UNBLOCK_SEQ_PRE_CSR_ADDR                                                             (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000008e4)
#define HWIO_RPMH_VRM_SW_UNBLOCK_SEQ_PRE_CSR_RMSK                                                             0xffffffff
#define HWIO_RPMH_VRM_SW_UNBLOCK_SEQ_PRE_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_SW_UNBLOCK_SEQ_PRE_CSR_ADDR,v)
#define HWIO_RPMH_VRM_SW_UNBLOCK_SEQ_PRE_CSR_UNBLOCK_SEQUENCER_PRE_BMSK                                       0xffffffff
#define HWIO_RPMH_VRM_SW_UNBLOCK_SEQ_PRE_CSR_UNBLOCK_SEQUENCER_PRE_SHFT                                              0x0

#define HWIO_RPMH_VRM_SW_UNBLOCK_SEQ_POST_CSR_ADDR                                                            (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000008e8)
#define HWIO_RPMH_VRM_SW_UNBLOCK_SEQ_POST_CSR_RMSK                                                            0xffffffff
#define HWIO_RPMH_VRM_SW_UNBLOCK_SEQ_POST_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_SW_UNBLOCK_SEQ_POST_CSR_ADDR,v)
#define HWIO_RPMH_VRM_SW_UNBLOCK_SEQ_POST_CSR_UNBLOCK_SEQUENCER_POST_BMSK                                     0xffffffff
#define HWIO_RPMH_VRM_SW_UNBLOCK_SEQ_POST_CSR_UNBLOCK_SEQUENCER_POST_SHFT                                            0x0

#define HWIO_RPMH_VRM_SEQUENCER_IRQ_CLEAR_PRE_CSR_ADDR                                                        (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000008ec)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_CLEAR_PRE_CSR_RMSK                                                        0xffffffff
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_CLEAR_PRE_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_SEQUENCER_IRQ_CLEAR_PRE_CSR_ADDR,v)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_CLEAR_PRE_CSR_SEQUENCER_IRQ_CLEAR_PRE_BMSK                                0xffffffff
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_CLEAR_PRE_CSR_SEQUENCER_IRQ_CLEAR_PRE_SHFT                                       0x0

#define HWIO_RPMH_VRM_SEQUENCER_IRQ_CLEAR_POST_CSR_ADDR                                                       (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000008f0)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_CLEAR_POST_CSR_RMSK                                                       0xffffffff
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_CLEAR_POST_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_SEQUENCER_IRQ_CLEAR_POST_CSR_ADDR,v)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_CLEAR_POST_CSR_SEQUENCER_IRQ_CLEAR_POST_BMSK                              0xffffffff
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_CLEAR_POST_CSR_SEQUENCER_IRQ_CLEAR_POST_SHFT                                     0x0

#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_PRE_CSR_ADDR                                                       (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000008f4)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_PRE_CSR_RMSK                                                       0xffffffff
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_PRE_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_PRE_CSR_ADDR, HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_PRE_CSR_RMSK)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_PRE_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_PRE_CSR_ADDR, m)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_PRE_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_PRE_CSR_ADDR,v)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_PRE_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_PRE_CSR_ADDR,m,v,HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_PRE_CSR_IN)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_PRE_CSR_SEQUENCER_IRQ_ENABLE_PRE_BMSK                              0xffffffff
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_PRE_CSR_SEQUENCER_IRQ_ENABLE_PRE_SHFT                                     0x0

#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_POST_CSR_ADDR                                                      (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000008f8)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_POST_CSR_RMSK                                                      0xffffffff
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_POST_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_POST_CSR_ADDR, HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_POST_CSR_RMSK)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_POST_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_POST_CSR_ADDR, m)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_POST_CSR_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_POST_CSR_ADDR,v)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_POST_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_POST_CSR_ADDR,m,v,HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_POST_CSR_IN)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_POST_CSR_SEQUENCER_IRQ_ENABLE_POST_BMSK                            0xffffffff
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_ENABLE_POST_CSR_SEQUENCER_IRQ_ENABLE_POST_SHFT                                   0x0

#define HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_PRE_CSR_ADDR                                                       (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x000008fc)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_PRE_CSR_RMSK                                                       0xffffffff
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_PRE_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_PRE_CSR_ADDR, HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_PRE_CSR_RMSK)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_PRE_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_PRE_CSR_ADDR, m)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_PRE_CSR_SEQUENCER_IRQ_STATUS_PRE_BMSK                              0xffffffff
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_PRE_CSR_SEQUENCER_IRQ_STATUS_PRE_SHFT                                     0x0

#define HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_POST_CSR_ADDR                                                      (RPMH_VRM_VRM_INTERRUPT_REG_BASE      + 0x00000900)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_POST_CSR_RMSK                                                      0xffffffff
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_POST_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_POST_CSR_ADDR, HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_POST_CSR_RMSK)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_POST_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_POST_CSR_ADDR, m)
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_POST_CSR_SEQUENCER_IRQ_STATUS_POST_BMSK                            0xffffffff
#define HWIO_RPMH_VRM_SEQUENCER_IRQ_STATUS_POST_CSR_SEQUENCER_IRQ_STATUS_POST_SHFT                                   0x0


#endif /* __VRM_INTERRUPT_HWIO_H__ */
