#ifndef __CPRF_HWIO_H__
#define __CPRF_HWIO_H__
/*
===========================================================================
*/
/**
  @file cprf_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r100_v3_BTO]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_CPRF_CPRF

  'Include' filters applied: VRM_SETTINGS[RPMH_CPRF_CPRF] VOLTAGE[RPMH_CPRF_CPRF] RPMH_CPRF_CPRFm_CTRL[RPMH_CPRF_CPRF] ENABLE[RPMH_CPRF_CPRF] 

  Generation parameters: 
  { u'filename': u'cprf_hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { u'RPMH_CPRF_CPRF': [ u'VRM_SETTINGS',
                                                     u'VOLTAGE',
                                                     u'RPMH_CPRF_CPRFm_CTRL',
                                                     u'ENABLE']},
    u'modules': [u'RPMH_CPRF_CPRF'],
    u'output-offsets': True,
    u'output-phys': True}
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/cpr/target/670/hwio/cprf_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
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


#endif /* __CPRF_HWIO_H__ */
