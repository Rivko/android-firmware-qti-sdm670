#ifndef __WCSSAON_HWIO_H__
#define __WCSSAON_HWIO_H__
/*
===========================================================================
*/
/**
  @file wcssaon_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r105_MTO]
 
  This file contains HWIO register definitions for the following modules:
    WCSSAON


  Generation parameters: 
  { u'filename': u'wcssaon_hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'WCSSAON'],
    u'output-fvals': True,
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/ddr_mgr/src/wcssaon_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: WCSSAON
 *--------------------------------------------------------------------------*/

#define WCSSAON_REG_BASE                                            (AOSS_BASE      + 0x01250000)
#define WCSSAON_REG_BASE_OFFS                                       0x01250000

#define HWIO_MPM_WCSSAON_CONFIG_ADDR                                (WCSSAON_REG_BASE      + 0x00000000)
#define HWIO_MPM_WCSSAON_CONFIG_OFFS                                (WCSSAON_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM_WCSSAON_CONFIG_RMSK                                  0x3ff3ff
#define HWIO_MPM_WCSSAON_CONFIG_IN          \
        in_dword_masked(HWIO_MPM_WCSSAON_CONFIG_ADDR, HWIO_MPM_WCSSAON_CONFIG_RMSK)
#define HWIO_MPM_WCSSAON_CONFIG_INM(m)      \
        in_dword_masked(HWIO_MPM_WCSSAON_CONFIG_ADDR, m)
#define HWIO_MPM_WCSSAON_CONFIG_OUT(v)      \
        out_dword(HWIO_MPM_WCSSAON_CONFIG_ADDR,v)
#define HWIO_MPM_WCSSAON_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM_WCSSAON_CONFIG_ADDR,m,v,HWIO_MPM_WCSSAON_CONFIG_IN)
#define HWIO_MPM_WCSSAON_CONFIG_DISCONNECT_CLR_BMSK                   0x200000
#define HWIO_MPM_WCSSAON_CONFIG_DISCONNECT_CLR_SHFT                       0x15
#define HWIO_MPM_WCSSAON_CONFIG_RCG_CLKGATE_DISABLE_BMSK              0x100000
#define HWIO_MPM_WCSSAON_CONFIG_RCG_CLKGATE_DISABLE_SHFT                  0x14
#define HWIO_MPM_WCSSAON_CONFIG_FORCE_XO_EN_BMSK                       0x80000
#define HWIO_MPM_WCSSAON_CONFIG_FORCE_XO_EN_SHFT                          0x13
#define HWIO_MPM_WCSSAON_CONFIG_TEST_BUS_SEL_BMSK                      0x78000
#define HWIO_MPM_WCSSAON_CONFIG_TEST_BUS_SEL_SHFT                          0xf
#define HWIO_MPM_WCSSAON_CONFIG_FORCE_ACTIVE_BMSK                       0x4000
#define HWIO_MPM_WCSSAON_CONFIG_FORCE_ACTIVE_SHFT                          0xe
#define HWIO_MPM_WCSSAON_CONFIG_CLAMP_MEM_ENABLE_BMSK                   0x2000
#define HWIO_MPM_WCSSAON_CONFIG_CLAMP_MEM_ENABLE_SHFT                      0xd
#define HWIO_MPM_WCSSAON_CONFIG_FORCE_RETENTION_BMSK                    0x1000
#define HWIO_MPM_WCSSAON_CONFIG_FORCE_RETENTION_SHFT                       0xc
#define HWIO_MPM_WCSSAON_CONFIG_CLK_EN_OVRD_VAL_BMSK                     0x200
#define HWIO_MPM_WCSSAON_CONFIG_CLK_EN_OVRD_VAL_SHFT                       0x9
#define HWIO_MPM_WCSSAON_CONFIG_CLK_EN_OVRD_BMSK                         0x100
#define HWIO_MPM_WCSSAON_CONFIG_CLK_EN_OVRD_SHFT                           0x8
#define HWIO_MPM_WCSSAON_CONFIG_MSM_CLAMP_EN_OVRD_VAL_BMSK                0x80
#define HWIO_MPM_WCSSAON_CONFIG_MSM_CLAMP_EN_OVRD_VAL_SHFT                 0x7
#define HWIO_MPM_WCSSAON_CONFIG_MSM_CLAMP_EN_OVRD_BMSK                    0x40
#define HWIO_MPM_WCSSAON_CONFIG_MSM_CLAMP_EN_OVRD_SHFT                     0x6
#define HWIO_MPM_WCSSAON_CONFIG_CLAMP_EN_OVRD_VAL_BMSK                    0x20
#define HWIO_MPM_WCSSAON_CONFIG_CLAMP_EN_OVRD_VAL_SHFT                     0x5
#define HWIO_MPM_WCSSAON_CONFIG_CLAMP_EN_OVRD_BMSK                        0x10
#define HWIO_MPM_WCSSAON_CONFIG_CLAMP_EN_OVRD_SHFT                         0x4
#define HWIO_MPM_WCSSAON_CONFIG_PWR_EN_OVRD_VAL_BMSK                       0x8
#define HWIO_MPM_WCSSAON_CONFIG_PWR_EN_OVRD_VAL_SHFT                       0x3
#define HWIO_MPM_WCSSAON_CONFIG_PWR_EN_OVRD_BMSK                           0x4
#define HWIO_MPM_WCSSAON_CONFIG_PWR_EN_OVRD_SHFT                           0x2
#define HWIO_MPM_WCSSAON_CONFIG_WLAN_DISABLE_BMSK                          0x2
#define HWIO_MPM_WCSSAON_CONFIG_WLAN_DISABLE_SHFT                          0x1
#define HWIO_MPM_WCSSAON_CONFIG_ARES_N_BMSK                                0x1
#define HWIO_MPM_WCSSAON_CONFIG_ARES_N_SHFT                                0x0

#define HWIO_WCSSAON_MPM_STATUS_ADDR                                (WCSSAON_REG_BASE      + 0x00000004)
#define HWIO_WCSSAON_MPM_STATUS_OFFS                                (WCSSAON_REG_BASE_OFFS + 0x00000004)
#define HWIO_WCSSAON_MPM_STATUS_RMSK                                0xffffffff
#define HWIO_WCSSAON_MPM_STATUS_IN          \
        in_dword_masked(HWIO_WCSSAON_MPM_STATUS_ADDR, HWIO_WCSSAON_MPM_STATUS_RMSK)
#define HWIO_WCSSAON_MPM_STATUS_INM(m)      \
        in_dword_masked(HWIO_WCSSAON_MPM_STATUS_ADDR, m)
#define HWIO_WCSSAON_MPM_STATUS_WCSSAON_STATUS_BMSK                 0xffffffff
#define HWIO_WCSSAON_MPM_STATUS_WCSSAON_STATUS_SHFT                        0x0


#endif /* __WCSSAON_HWIO_H__ */
