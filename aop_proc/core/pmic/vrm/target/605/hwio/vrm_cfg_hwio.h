#ifndef __VRM_CFG_HWIO_H__
#define __VRM_CFG_HWIO_H__
/*
===========================================================================
*/
/**
  @file vrm_cfg_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) v2 [napali_v2.0_p3q2r35_MTO_with_AOSS_USB_PHY_ECOs_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_VRM_VRM_CFG


  Generation parameters: 
  { u'filename': u'vrm_cfg_hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'RPMH_VRM_VRM_CFG']}
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/target/605/hwio/vrm_cfg_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: RPMH_VRM_VRM_CFG
 *--------------------------------------------------------------------------*/

#define RPMH_VRM_VRM_CFG_REG_BASE                                                             (AOSS_BASE      + 0x009f5000)

#define HWIO_RPMH_VRM_VRM_VERSION_ADDR                                                        (RPMH_VRM_VRM_CFG_REG_BASE      + 0x00000000)
#define HWIO_RPMH_VRM_VRM_VERSION_RMSK                                                          0xffffff
#define HWIO_RPMH_VRM_VRM_VERSION_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VRM_VERSION_ADDR, HWIO_RPMH_VRM_VRM_VERSION_RMSK)
#define HWIO_RPMH_VRM_VRM_VERSION_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VRM_VERSION_ADDR, m)
#define HWIO_RPMH_VRM_VRM_VERSION_MAJOR_VERSION_BMSK                                            0xff0000
#define HWIO_RPMH_VRM_VRM_VERSION_MAJOR_VERSION_SHFT                                                0x10
#define HWIO_RPMH_VRM_VRM_VERSION_MINOR_VERSION_BMSK                                              0xff00
#define HWIO_RPMH_VRM_VRM_VERSION_MINOR_VERSION_SHFT                                                 0x8
#define HWIO_RPMH_VRM_VRM_VERSION_STEP_VERSION_BMSK                                                 0xff
#define HWIO_RPMH_VRM_VRM_VERSION_STEP_VERSION_SHFT                                                  0x0

#define HWIO_RPMH_VRM_VREG_READY_EXTRA_TIME_ADDR                                              (RPMH_VRM_VRM_CFG_REG_BASE      + 0x00000004)
#define HWIO_RPMH_VRM_VREG_READY_EXTRA_TIME_RMSK                                                    0xff
#define HWIO_RPMH_VRM_VREG_READY_EXTRA_TIME_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_EXTRA_TIME_ADDR, HWIO_RPMH_VRM_VREG_READY_EXTRA_TIME_RMSK)
#define HWIO_RPMH_VRM_VREG_READY_EXTRA_TIME_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_EXTRA_TIME_ADDR, m)
#define HWIO_RPMH_VRM_VREG_READY_EXTRA_TIME_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VREG_READY_EXTRA_TIME_ADDR,v)
#define HWIO_RPMH_VRM_VREG_READY_EXTRA_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VREG_READY_EXTRA_TIME_ADDR,m,v,HWIO_RPMH_VRM_VREG_READY_EXTRA_TIME_IN)
#define HWIO_RPMH_VRM_VREG_READY_EXTRA_TIME_VREG_READY_EXTRA_TIME_BMSK                              0xff
#define HWIO_RPMH_VRM_VREG_READY_EXTRA_TIME_VREG_READY_EXTRA_TIME_SHFT                               0x0

#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_ADDR                                       (RPMH_VRM_VRM_CFG_REG_BASE      + 0x00000008)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_RMSK                                              0x1
#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_ADDR, HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_RMSK)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_ADDR, m)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_ADDR,v)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_ADDR,m,v,HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_IN)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_VREG_READY_WDOG_ERROR_ENABLE_BMSK                 0x1
#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_VREG_READY_WDOG_ERROR_ENABLE_SHFT                 0x0

#define HWIO_RPMH_VRM_VREG_READY_CLR_BR_ADDR_CSR_REG_ADDR                                     (RPMH_VRM_VRM_CFG_REG_BASE      + 0x0000000c)
#define HWIO_RPMH_VRM_VREG_READY_CLR_BR_ADDR_CSR_REG_RMSK                                          0x3ff
#define HWIO_RPMH_VRM_VREG_READY_CLR_BR_ADDR_CSR_REG_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_CLR_BR_ADDR_CSR_REG_ADDR, HWIO_RPMH_VRM_VREG_READY_CLR_BR_ADDR_CSR_REG_RMSK)
#define HWIO_RPMH_VRM_VREG_READY_CLR_BR_ADDR_CSR_REG_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_CLR_BR_ADDR_CSR_REG_ADDR, m)
#define HWIO_RPMH_VRM_VREG_READY_CLR_BR_ADDR_CSR_REG_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VREG_READY_CLR_BR_ADDR_CSR_REG_ADDR,v)
#define HWIO_RPMH_VRM_VREG_READY_CLR_BR_ADDR_CSR_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VREG_READY_CLR_BR_ADDR_CSR_REG_ADDR,m,v,HWIO_RPMH_VRM_VREG_READY_CLR_BR_ADDR_CSR_REG_IN)
#define HWIO_RPMH_VRM_VREG_READY_CLR_BR_ADDR_CSR_REG_VREG_CLR_BR_ADDR_BMSK                         0x3ff
#define HWIO_RPMH_VRM_VREG_READY_CLR_BR_ADDR_CSR_REG_VREG_CLR_BR_ADDR_SHFT                           0x0

#define HWIO_RPMH_VRM_CMD_SYN_UNBLOCK_ADDR                                                    (RPMH_VRM_VRM_CFG_REG_BASE      + 0x00000010)
#define HWIO_RPMH_VRM_CMD_SYN_UNBLOCK_RMSK                                                    0xffffffff
#define HWIO_RPMH_VRM_CMD_SYN_UNBLOCK_IN          \
        in_dword_masked(HWIO_RPMH_VRM_CMD_SYN_UNBLOCK_ADDR, HWIO_RPMH_VRM_CMD_SYN_UNBLOCK_RMSK)
#define HWIO_RPMH_VRM_CMD_SYN_UNBLOCK_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_CMD_SYN_UNBLOCK_ADDR, m)
#define HWIO_RPMH_VRM_CMD_SYN_UNBLOCK_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_CMD_SYN_UNBLOCK_ADDR,v)
#define HWIO_RPMH_VRM_CMD_SYN_UNBLOCK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_CMD_SYN_UNBLOCK_ADDR,m,v,HWIO_RPMH_VRM_CMD_SYN_UNBLOCK_IN)
#define HWIO_RPMH_VRM_CMD_SYN_UNBLOCK_CMD_SYN_UNBLOCK_BMSK                                    0xffffffff
#define HWIO_RPMH_VRM_CMD_SYN_UNBLOCK_CMD_SYN_UNBLOCK_SHFT                                           0x0

#define HWIO_RPMH_VRM_MIN_SETTLING_TIME_CSR_REG_ADDR                                          (RPMH_VRM_VRM_CFG_REG_BASE      + 0x00000014)
#define HWIO_RPMH_VRM_MIN_SETTLING_TIME_CSR_REG_RMSK                                              0xffff
#define HWIO_RPMH_VRM_MIN_SETTLING_TIME_CSR_REG_IN          \
        in_dword_masked(HWIO_RPMH_VRM_MIN_SETTLING_TIME_CSR_REG_ADDR, HWIO_RPMH_VRM_MIN_SETTLING_TIME_CSR_REG_RMSK)
#define HWIO_RPMH_VRM_MIN_SETTLING_TIME_CSR_REG_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_MIN_SETTLING_TIME_CSR_REG_ADDR, m)
#define HWIO_RPMH_VRM_MIN_SETTLING_TIME_CSR_REG_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_MIN_SETTLING_TIME_CSR_REG_ADDR,v)
#define HWIO_RPMH_VRM_MIN_SETTLING_TIME_CSR_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MIN_SETTLING_TIME_CSR_REG_ADDR,m,v,HWIO_RPMH_VRM_MIN_SETTLING_TIME_CSR_REG_IN)
#define HWIO_RPMH_VRM_MIN_SETTLING_TIME_CSR_REG_MIN_SETTLING_TIME_BMSK                            0xffff
#define HWIO_RPMH_VRM_MIN_SETTLING_TIME_CSR_REG_MIN_SETTLING_TIME_SHFT                               0x0

#define HWIO_RPMH_VRM_RESP_ADDR_DRVr_ADDR(r)                                                  (RPMH_VRM_VRM_CFG_REG_BASE      + 0x00000090 + 0x4 * (r))
#define HWIO_RPMH_VRM_RESP_ADDR_DRVr_RMSK                                                     0xffffffff
#define HWIO_RPMH_VRM_RESP_ADDR_DRVr_MAXr                                                             13
#define HWIO_RPMH_VRM_RESP_ADDR_DRVr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_RESP_ADDR_DRVr_ADDR(r), HWIO_RPMH_VRM_RESP_ADDR_DRVr_RMSK)
#define HWIO_RPMH_VRM_RESP_ADDR_DRVr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_RESP_ADDR_DRVr_ADDR(r), mask)
#define HWIO_RPMH_VRM_RESP_ADDR_DRVr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_RESP_ADDR_DRVr_ADDR(r),val)
#define HWIO_RPMH_VRM_RESP_ADDR_DRVr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_RESP_ADDR_DRVr_ADDR(r),mask,val,HWIO_RPMH_VRM_RESP_ADDR_DRVr_INI(r))
#define HWIO_RPMH_VRM_RESP_ADDR_DRVr_RESP_ADDR_BMSK                                           0xffffffff
#define HWIO_RPMH_VRM_RESP_ADDR_DRVr_RESP_ADDR_SHFT                                                  0x0

#define HWIO_RPMH_VRM_RESP_ADDR2_DRVr_ADDR(r)                                                 (RPMH_VRM_VRM_CFG_REG_BASE      + 0x00000100 + 0x4 * (r))
#define HWIO_RPMH_VRM_RESP_ADDR2_DRVr_RMSK                                                    0xffffffff
#define HWIO_RPMH_VRM_RESP_ADDR2_DRVr_MAXr                                                            13
#define HWIO_RPMH_VRM_RESP_ADDR2_DRVr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_RESP_ADDR2_DRVr_ADDR(r), HWIO_RPMH_VRM_RESP_ADDR2_DRVr_RMSK)
#define HWIO_RPMH_VRM_RESP_ADDR2_DRVr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_RESP_ADDR2_DRVr_ADDR(r), mask)
#define HWIO_RPMH_VRM_RESP_ADDR2_DRVr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_RESP_ADDR2_DRVr_ADDR(r),val)
#define HWIO_RPMH_VRM_RESP_ADDR2_DRVr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_RESP_ADDR2_DRVr_ADDR(r),mask,val,HWIO_RPMH_VRM_RESP_ADDR2_DRVr_INI(r))
#define HWIO_RPMH_VRM_RESP_ADDR2_DRVr_RESP_ADDR2_BMSK                                         0xffffffff
#define HWIO_RPMH_VRM_RESP_ADDR2_DRVr_RESP_ADDR2_SHFT                                                0x0

#define HWIO_RPMH_VRM_VRM_CFG_ADDR                                                            (RPMH_VRM_VRM_CFG_REG_BASE      + 0x000004a8)
#define HWIO_RPMH_VRM_VRM_CFG_RMSK                                                                 0x1fd
#define HWIO_RPMH_VRM_VRM_CFG_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VRM_CFG_ADDR, HWIO_RPMH_VRM_VRM_CFG_RMSK)
#define HWIO_RPMH_VRM_VRM_CFG_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VRM_CFG_ADDR, m)
#define HWIO_RPMH_VRM_VRM_CFG_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VRM_CFG_ADDR,v)
#define HWIO_RPMH_VRM_VRM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VRM_CFG_ADDR,m,v,HWIO_RPMH_VRM_VRM_CFG_IN)
#define HWIO_RPMH_VRM_VRM_CFG_DBG_EN_BMSK                                                          0x100
#define HWIO_RPMH_VRM_VRM_CFG_DBG_EN_SHFT                                                            0x8
#define HWIO_RPMH_VRM_VRM_CFG_SLAVE_ID_BMSK                                                         0xfc
#define HWIO_RPMH_VRM_VRM_CFG_SLAVE_ID_SHFT                                                          0x2
#define HWIO_RPMH_VRM_VRM_CFG_VRM_EN_BMSK                                                            0x1
#define HWIO_RPMH_VRM_VRM_CFG_VRM_EN_SHFT                                                            0x0

#define HWIO_RPMH_VRM_CMD_SYN_CFG_ADDR                                                        (RPMH_VRM_VRM_CFG_REG_BASE      + 0x000004ac)
#define HWIO_RPMH_VRM_CMD_SYN_CFG_RMSK                                                               0x2
#define HWIO_RPMH_VRM_CMD_SYN_CFG_IN          \
        in_dword_masked(HWIO_RPMH_VRM_CMD_SYN_CFG_ADDR, HWIO_RPMH_VRM_CMD_SYN_CFG_RMSK)
#define HWIO_RPMH_VRM_CMD_SYN_CFG_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_CMD_SYN_CFG_ADDR, m)
#define HWIO_RPMH_VRM_CMD_SYN_CFG_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_CMD_SYN_CFG_ADDR,v)
#define HWIO_RPMH_VRM_CMD_SYN_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_CMD_SYN_CFG_ADDR,m,v,HWIO_RPMH_VRM_CMD_SYN_CFG_IN)
#define HWIO_RPMH_VRM_CMD_SYN_CFG_VHM_EN_BMSK                                                        0x2
#define HWIO_RPMH_VRM_CMD_SYN_CFG_VHM_EN_SHFT                                                        0x1

#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR0_CSR_REG_ADDR                                     (RPMH_VRM_VRM_CFG_REG_BASE      + 0x000004b0)
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR0_CSR_REG_RMSK                                          0x3ff
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR0_CSR_REG_IN          \
        in_dword_masked(HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR0_CSR_REG_ADDR, HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR0_CSR_REG_RMSK)
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR0_CSR_REG_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR0_CSR_REG_ADDR, m)
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR0_CSR_REG_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR0_CSR_REG_ADDR,v)
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR0_CSR_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR0_CSR_REG_ADDR,m,v,HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR0_CSR_REG_IN)
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR0_CSR_REG_BR_ADDR0_BMSK                                 0x3ff
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR0_CSR_REG_BR_ADDR0_SHFT                                   0x0

#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR1_CSR_REG_ADDR                                     (RPMH_VRM_VRM_CFG_REG_BASE      + 0x000004b4)
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR1_CSR_REG_RMSK                                          0x3ff
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR1_CSR_REG_IN          \
        in_dword_masked(HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR1_CSR_REG_ADDR, HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR1_CSR_REG_RMSK)
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR1_CSR_REG_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR1_CSR_REG_ADDR, m)
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR1_CSR_REG_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR1_CSR_REG_ADDR,v)
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR1_CSR_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR1_CSR_REG_ADDR,m,v,HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR1_CSR_REG_IN)
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR1_CSR_REG_BR_ADDR1_BMSK                                 0x3ff
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR1_CSR_REG_BR_ADDR1_SHFT                                   0x0

#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR2_CSR_REG_ADDR                                     (RPMH_VRM_VRM_CFG_REG_BASE      + 0x000004b8)
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR2_CSR_REG_RMSK                                          0x3ff
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR2_CSR_REG_IN          \
        in_dword_masked(HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR2_CSR_REG_ADDR, HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR2_CSR_REG_RMSK)
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR2_CSR_REG_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR2_CSR_REG_ADDR, m)
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR2_CSR_REG_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR2_CSR_REG_ADDR,v)
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR2_CSR_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR2_CSR_REG_ADDR,m,v,HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR2_CSR_REG_IN)
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR2_CSR_REG_BR_ADDR2_BMSK                                 0x3ff
#define HWIO_RPMH_VRM_SW_WORKAROUND_BR_ADDR2_CSR_REG_BR_ADDR2_SHFT                                   0x0

#define HWIO_RPMH_VRM_VRM_CLK_GATING_EN_CFG_ADDR                                              (RPMH_VRM_VRM_CFG_REG_BASE      + 0x000004bc)
#define HWIO_RPMH_VRM_VRM_CLK_GATING_EN_CFG_RMSK                                                     0x1
#define HWIO_RPMH_VRM_VRM_CLK_GATING_EN_CFG_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VRM_CLK_GATING_EN_CFG_ADDR, HWIO_RPMH_VRM_VRM_CLK_GATING_EN_CFG_RMSK)
#define HWIO_RPMH_VRM_VRM_CLK_GATING_EN_CFG_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VRM_CLK_GATING_EN_CFG_ADDR, m)
#define HWIO_RPMH_VRM_VRM_CLK_GATING_EN_CFG_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VRM_CLK_GATING_EN_CFG_ADDR,v)
#define HWIO_RPMH_VRM_VRM_CLK_GATING_EN_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VRM_CLK_GATING_EN_CFG_ADDR,m,v,HWIO_RPMH_VRM_VRM_CLK_GATING_EN_CFG_IN)
#define HWIO_RPMH_VRM_VRM_CLK_GATING_EN_CFG_VRM_CLK_GATING_EN_BMSK                                   0x1
#define HWIO_RPMH_VRM_VRM_CLK_GATING_EN_CFG_VRM_CLK_GATING_EN_SHFT                                   0x0

#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_EN_CFG_ADDR                                               (RPMH_VRM_VRM_CFG_REG_BASE      + 0x000004c0)
#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_EN_CFG_RMSK                                                      0x1
#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_EN_CFG_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VRM_DEBUG_BUS_EN_CFG_ADDR, HWIO_RPMH_VRM_VRM_DEBUG_BUS_EN_CFG_RMSK)
#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_EN_CFG_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VRM_DEBUG_BUS_EN_CFG_ADDR, m)
#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_EN_CFG_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VRM_DEBUG_BUS_EN_CFG_ADDR,v)
#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_EN_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VRM_DEBUG_BUS_EN_CFG_ADDR,m,v,HWIO_RPMH_VRM_VRM_DEBUG_BUS_EN_CFG_IN)
#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_EN_CFG_VRM_DEBUG_BUS_EN_BMSK                                     0x1
#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_EN_CFG_VRM_DEBUG_BUS_EN_SHFT                                     0x0

#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_CFG_ADDR                                                  (RPMH_VRM_VRM_CFG_REG_BASE      + 0x000004c4)
#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_CFG_RMSK                                                        0xff
#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_CFG_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VRM_DEBUG_BUS_CFG_ADDR, HWIO_RPMH_VRM_VRM_DEBUG_BUS_CFG_RMSK)
#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_CFG_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VRM_DEBUG_BUS_CFG_ADDR, m)
#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_CFG_OUT(v)      \
        out_dword(HWIO_RPMH_VRM_VRM_DEBUG_BUS_CFG_ADDR,v)
#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VRM_DEBUG_BUS_CFG_ADDR,m,v,HWIO_RPMH_VRM_VRM_DEBUG_BUS_CFG_IN)
#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_CFG_RES_SEL_BMSK                                                0xff
#define HWIO_RPMH_VRM_VRM_DEBUG_BUS_CFG_RES_SEL_SHFT                                                 0x0


#endif /* __VRM_CFG_HWIO_H__ */
