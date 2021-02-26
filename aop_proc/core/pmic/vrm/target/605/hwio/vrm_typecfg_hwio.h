#ifndef __VRM_TYPECFG_HWIO_H__
#define __VRM_TYPECFG_HWIO_H__
/*
===========================================================================
*/
/**
  @file vrm_typecfg_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) v2 [napali_v2.0_p3q2r35_MTO_with_AOSS_USB_PHY_ECOs_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_VRM_VRM_TYPE_CFG


  Generation parameters: 
  { u'filename': u'vrm_typecfg_hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'RPMH_VRM_VRM_TYPE_CFG']}
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/target/605/hwio/vrm_typecfg_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: RPMH_VRM_VRM_TYPE_CFG
 *--------------------------------------------------------------------------*/

#define RPMH_VRM_VRM_TYPE_CFG_REG_BASE                                                                          (AOSS_BASE      + 0x009f0000)

#define HWIO_RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr_ADDR(r)                                                     (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000000 + 0x60 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr_RMSK                                                              0xff
#define HWIO_RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr_MAXr                                                                15
#define HWIO_RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr_VOLTAGE_ADDR_OFFSET_BMSK                                          0xff
#define HWIO_RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr_VOLTAGE_ADDR_OFFSET_SHFT                                           0x0

#define HWIO_RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr_ADDR(r)                                                      (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000004 + 0x60 * (r))
#define HWIO_RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr_RMSK                                                               0xff
#define HWIO_RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr_MAXr                                                                 15
#define HWIO_RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr_ENABLE_ADDR_OFFSET_BMSK                                            0xff
#define HWIO_RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr_ENABLE_ADDR_OFFSET_SHFT                                             0x0

#define HWIO_RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr_ADDR(r)                                                        (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000008 + 0x60 * (r))
#define HWIO_RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr_RMSK                                                                 0xff
#define HWIO_RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr_MAXr                                                                   15
#define HWIO_RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr_MODE_ADDR_OFFSET_BMSK                                                0xff
#define HWIO_RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr_MODE_ADDR_OFFSET_SHFT                                                 0x0

#define HWIO_RPMH_VRM_HEADROOM_ADDR_OFFSET_CSR_TYPEr_ADDR(r)                                                    (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x0000000c + 0x60 * (r))
#define HWIO_RPMH_VRM_HEADROOM_ADDR_OFFSET_CSR_TYPEr_RMSK                                                             0xff
#define HWIO_RPMH_VRM_HEADROOM_ADDR_OFFSET_CSR_TYPEr_MAXr                                                               15
#define HWIO_RPMH_VRM_HEADROOM_ADDR_OFFSET_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_ADDR_OFFSET_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_HEADROOM_ADDR_OFFSET_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_ADDR_OFFSET_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_ADDR_OFFSET_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_HEADROOM_ADDR_OFFSET_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_HEADROOM_ADDR_OFFSET_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_HEADROOM_ADDR_OFFSET_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_ADDR_OFFSET_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_HEADROOM_ADDR_OFFSET_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_HEADROOM_ADDR_OFFSET_CSR_TYPEr_HEADROOM_ADDR_OFFSET_BMSK                                        0xff
#define HWIO_RPMH_VRM_HEADROOM_ADDR_OFFSET_CSR_TYPEr_HEADROOM_ADDR_OFFSET_SHFT                                         0x0

#define HWIO_RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr_ADDR(r)                                           (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x0000001c + 0x60 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr_RMSK                                                  0xffff
#define HWIO_RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr_MAXr                                                      15
#define HWIO_RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr_VOLTAGE_STEPPER_SETTLING_TIME_BMSK                    0xffff
#define HWIO_RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr_VOLTAGE_STEPPER_SETTLING_TIME_SHFT                       0x0

#define HWIO_RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr_ADDR(r)                                          (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000020 + 0x60 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr_RMSK                                                 0xffff
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr_MAXr                                                     15
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr_VOLTAGE_FIXED_UP_SETTLING_TIME_BMSK                  0xffff
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr_VOLTAGE_FIXED_UP_SETTLING_TIME_SHFT                     0x0

#define HWIO_RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr_ADDR(r)                                        (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000024 + 0x60 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr_RMSK                                               0xffff
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr_MAXr                                                   15
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr_VOLTAGE_FIXED_DOWN_SETTLING_TIME_BMSK              0xffff
#define HWIO_RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr_VOLTAGE_FIXED_DOWN_SETTLING_TIME_SHFT                 0x0

#define HWIO_RPMH_VRM_VOLTAGE_UP_SETTLING_TIME_SEL_ADDR                                                         (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000ff0)
#define HWIO_RPMH_VRM_VOLTAGE_UP_SETTLING_TIME_SEL_RMSK                                                             0xffff
#define HWIO_RPMH_VRM_VOLTAGE_UP_SETTLING_TIME_SEL_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_UP_SETTLING_TIME_SEL_ADDR, HWIO_RPMH_VRM_VOLTAGE_UP_SETTLING_TIME_SEL_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_UP_SETTLING_TIME_SEL_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_UP_SETTLING_TIME_SEL_ADDR, m)
#define HWIO_RPMH_VRM_VOLTAGE_UP_SETTLING_TIME_SEL_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_UP_SETTLING_TIME_SEL_ADDR,v)
#define HWIO_RPMH_VRM_VOLTAGE_UP_SETTLING_TIME_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_UP_SETTLING_TIME_SEL_ADDR,m,v,HWIO_RPMH_VRM_VOLTAGE_UP_SETTLING_TIME_SEL_IN)
#define HWIO_RPMH_VRM_VOLTAGE_UP_SETTLING_TIME_SEL_VOLTAGE_UP_SETTLING_TIME_SEL_BMSK                                0xffff
#define HWIO_RPMH_VRM_VOLTAGE_UP_SETTLING_TIME_SEL_VOLTAGE_UP_SETTLING_TIME_SEL_SHFT                                   0x0

#define HWIO_RPMH_VRM_VOLTAGE_DOWN_SETTLING_TIME_SEL_ADDR                                                       (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000ff4)
#define HWIO_RPMH_VRM_VOLTAGE_DOWN_SETTLING_TIME_SEL_RMSK                                                           0xffff
#define HWIO_RPMH_VRM_VOLTAGE_DOWN_SETTLING_TIME_SEL_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DOWN_SETTLING_TIME_SEL_ADDR, HWIO_RPMH_VRM_VOLTAGE_DOWN_SETTLING_TIME_SEL_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_DOWN_SETTLING_TIME_SEL_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DOWN_SETTLING_TIME_SEL_ADDR, m)
#define HWIO_RPMH_VRM_VOLTAGE_DOWN_SETTLING_TIME_SEL_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_DOWN_SETTLING_TIME_SEL_ADDR,v)
#define HWIO_RPMH_VRM_VOLTAGE_DOWN_SETTLING_TIME_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_DOWN_SETTLING_TIME_SEL_ADDR,m,v,HWIO_RPMH_VRM_VOLTAGE_DOWN_SETTLING_TIME_SEL_IN)
#define HWIO_RPMH_VRM_VOLTAGE_DOWN_SETTLING_TIME_SEL_VOLTAGE_DOWN_SETTLING_TIME_SEL_BMSK                            0xffff
#define HWIO_RPMH_VRM_VOLTAGE_DOWN_SETTLING_TIME_SEL_VOLTAGE_DOWN_SETTLING_TIME_SEL_SHFT                               0x0

#define HWIO_RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr_ADDR(r)                                            (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000028 + 0x60 * (r))
#define HWIO_RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr_RMSK                                                   0xffff
#define HWIO_RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr_MAXr                                                       15
#define HWIO_RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr_ENABLE_STEPPER_SETTLING_TIME_BMSK                      0xffff
#define HWIO_RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr_ENABLE_STEPPER_SETTLING_TIME_SHFT                         0x0

#define HWIO_RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr_ADDR(r)                                              (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x0000002c + 0x60 * (r))
#define HWIO_RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr_RMSK                                                     0xffff
#define HWIO_RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr_MAXr                                                         15
#define HWIO_RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr_ENABLE_FIXED_SETTLING_TIME_BMSK                          0xffff
#define HWIO_RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr_ENABLE_FIXED_SETTLING_TIME_SHFT                             0x0

#define HWIO_RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr_ADDR(r)                                                     (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000030 + 0x60 * (r))
#define HWIO_RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr_RMSK                                                            0xffff
#define HWIO_RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr_MAXr                                                                15
#define HWIO_RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr_ENABLE_WARM_UP_TIME_BMSK                                        0xffff
#define HWIO_RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr_ENABLE_WARM_UP_TIME_SHFT                                           0x0

#define HWIO_RPMH_VRM_ENABLE_UP_SETTLING_TIME_SEL_ADDR                                                          (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000ff8)
#define HWIO_RPMH_VRM_ENABLE_UP_SETTLING_TIME_SEL_RMSK                                                              0xffff
#define HWIO_RPMH_VRM_ENABLE_UP_SETTLING_TIME_SEL_IN          \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_UP_SETTLING_TIME_SEL_ADDR, HWIO_RPMH_VRM_ENABLE_UP_SETTLING_TIME_SEL_RMSK)
#define HWIO_RPMH_VRM_ENABLE_UP_SETTLING_TIME_SEL_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_UP_SETTLING_TIME_SEL_ADDR, m)
#define HWIO_RPMH_VRM_ENABLE_UP_SETTLING_TIME_SEL_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_ENABLE_UP_SETTLING_TIME_SEL_ADDR,v)
#define HWIO_RPMH_VRM_ENABLE_UP_SETTLING_TIME_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_UP_SETTLING_TIME_SEL_ADDR,m,v,HWIO_RPMH_VRM_ENABLE_UP_SETTLING_TIME_SEL_IN)
#define HWIO_RPMH_VRM_ENABLE_UP_SETTLING_TIME_SEL_ENABLE_UP_SETTLING_TIME_SEL_BMSK                                  0xffff
#define HWIO_RPMH_VRM_ENABLE_UP_SETTLING_TIME_SEL_ENABLE_UP_SETTLING_TIME_SEL_SHFT                                     0x0

#define HWIO_RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr_ADDR(r)                                                      (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000034 + 0x60 * (r))
#define HWIO_RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr_RMSK                                                             0xffff
#define HWIO_RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr_MAXr                                                                 15
#define HWIO_RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr_MODE_SETTLING_TIME_BMSK                                          0xffff
#define HWIO_RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr_MODE_SETTLING_TIME_SHFT                                             0x0

#define HWIO_RPMH_VRM_HEADROOM_SETTLING_TIME_CSR_TYPEr_ADDR(r)                                                  (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000038 + 0x60 * (r))
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_TIME_CSR_TYPEr_RMSK                                                         0xffff
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_TIME_CSR_TYPEr_MAXr                                                             15
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_TIME_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_SETTLING_TIME_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_HEADROOM_SETTLING_TIME_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_TIME_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_SETTLING_TIME_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_TIME_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_HEADROOM_SETTLING_TIME_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_TIME_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_SETTLING_TIME_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_HEADROOM_SETTLING_TIME_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_TIME_CSR_TYPEr_HEADROOM_SETTLING_TIME_BMSK                                  0xffff
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_TIME_CSR_TYPEr_HEADROOM_SETTLING_TIME_SHFT                                     0x0

#define HWIO_RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr_ADDR(r)                                               (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x0000003c + 0x60 * (r))
#define HWIO_RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr_RMSK                                                         0x7
#define HWIO_RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr_MAXr                                                          15
#define HWIO_RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr_VREG_READY_WDOG_TIMER_MUL_BMSK                               0x7
#define HWIO_RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr_VREG_READY_WDOG_TIMER_MUL_SHFT                               0x0

#define HWIO_RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr_ADDR(r)                                                       (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000040 + 0x60 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr_RMSK                                                              0xffff
#define HWIO_RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr_MAXr                                                                  15
#define HWIO_RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr_ADDR(r), HWIO_RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr_INI(r))
#define HWIO_RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr_VOLTAGE_UP_EXTRA_TIME_BMSK                                        0xffff
#define HWIO_RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr_VOLTAGE_UP_EXTRA_TIME_SHFT                                           0x0

#define HWIO_RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr_ADDR(r)                                                     (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000044 + 0x60 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr_RMSK                                                            0xffff
#define HWIO_RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr_MAXr                                                                15
#define HWIO_RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr_ADDR(r), HWIO_RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr_INI(r))
#define HWIO_RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr_VOLTAGE_DOWN_EXTRA_TIME_BMSK                                    0xffff
#define HWIO_RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr_VOLTAGE_DOWN_EXTRA_TIME_SHFT                                       0x0

#define HWIO_RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr_ADDR(r)                                                    (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x0000004c + 0x60 * (r))
#define HWIO_RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr_RMSK                                                             0xff
#define HWIO_RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr_MAXr                                                               15
#define HWIO_RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr_VREG_CLR_ADDR_OFFSET_BMSK                                        0xff
#define HWIO_RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr_VREG_CLR_ADDR_OFFSET_SHFT                                         0x0

#define HWIO_RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr_ADDR(r)                                                           (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000050 + 0x60 * (r))
#define HWIO_RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr_RMSK                                                                    0xff
#define HWIO_RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr_MAXr                                                                      15
#define HWIO_RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr_VREG_CLR_DATA_BMSK                                                      0xff
#define HWIO_RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr_VREG_CLR_DATA_SHFT                                                       0x0

#define HWIO_RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr_ADDR(r)                                                          (RPMH_VRM_VRM_TYPE_CFG_REG_BASE      + 0x00000054 + 0x60 * (r))
#define HWIO_RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr_RMSK                                                                 0xffff
#define HWIO_RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr_MAXr                                                                     15
#define HWIO_RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr_ADDR(r), HWIO_RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr_RMSK)
#define HWIO_RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr_ADDR(r), mask)
#define HWIO_RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr_ADDR(r),val)
#define HWIO_RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr_ADDR(r),mask,val,HWIO_RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr_INI(r))
#define HWIO_RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr_DISCHARGE_TIME_BMSK                                                  0xffff
#define HWIO_RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr_DISCHARGE_TIME_SHFT                                                     0x0


#endif /* __VRM_TYPECFG_HWIO_H__ */
