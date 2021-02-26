#ifndef __VRM_RESCFG_HWIO_H__
#define __VRM_RESCFG_HWIO_H__
/*
===========================================================================
*/
/**
  @file vrm_rescfg_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) v2 [napali_v2.0_p3q2r35_MTO_with_AOSS_USB_PHY_ECOs_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_VRM_VRM_RES_CFG


  Generation parameters: 
  { u'filename': u'vrm_rescfg_hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'RPMH_VRM_VRM_RES_CFG']}
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/target/605/hwio/vrm_rescfg_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: RPMH_VRM_VRM_RES_CFG
 *--------------------------------------------------------------------------*/

#define RPMH_VRM_VRM_RES_CFG_REG_BASE                                                                              (AOSS_BASE      + 0x009e0000)

#define HWIO_RPMH_VRM_VOLTAGE_CRS_CSR_REGr_ADDR(r)                                                                 (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000000 + 0x100 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_CRS_CSR_REGr_RMSK                                                                        0x1fff
#define HWIO_RPMH_VRM_VOLTAGE_CRS_CSR_REGr_MAXr                                                                            45
#define HWIO_RPMH_VRM_VOLTAGE_CRS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_CRS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_VOLTAGE_CRS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_CRS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_CRS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_CRS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_CRS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_VOLTAGE_CRS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_CRS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_VOLTAGE_CRS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_VOLTAGE_CRS_CSR_REGr_VOLTAGE_BMSK                                                                0x1fff
#define HWIO_RPMH_VRM_VOLTAGE_CRS_CSR_REGr_VOLTAGE_SHFT                                                                   0x0

#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_REGr_ADDR(r)                                                                  (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000004 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_REGr_RMSK                                                                            0x1
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_REGr_MAXr                                                                             45
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CRS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_ENABLE_CRS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CRS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_CRS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_CRS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_CRS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_REGr_ENABLE_BMSK                                                                     0x1
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_REGr_ENABLE_SHFT                                                                     0x0

#define HWIO_RPMH_VRM_MODE_CRS_CSR_REGr_ADDR(r)                                                                    (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000008 + 0x100 * (r))
#define HWIO_RPMH_VRM_MODE_CRS_CSR_REGr_RMSK                                                                              0x7
#define HWIO_RPMH_VRM_MODE_CRS_CSR_REGr_MAXr                                                                               45
#define HWIO_RPMH_VRM_MODE_CRS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_MODE_CRS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_MODE_CRS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_MODE_CRS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_MODE_CRS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_MODE_CRS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_MODE_CRS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_MODE_CRS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_CRS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_MODE_CRS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_MODE_CRS_CSR_REGr_MODE_BMSK                                                                         0x7
#define HWIO_RPMH_VRM_MODE_CRS_CSR_REGr_MODE_SHFT                                                                         0x0

#define HWIO_RPMH_VRM_HEADROOM_CRS_CSR_REGr_ADDR(r)                                                                (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000000c + 0x100 * (r))
#define HWIO_RPMH_VRM_HEADROOM_CRS_CSR_REGr_RMSK                                                                        0x1ff
#define HWIO_RPMH_VRM_HEADROOM_CRS_CSR_REGr_MAXr                                                                           45
#define HWIO_RPMH_VRM_HEADROOM_CRS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_CRS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_HEADROOM_CRS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_CRS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_CRS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_HEADROOM_CRS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_HEADROOM_CRS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_HEADROOM_CRS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_CRS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_HEADROOM_CRS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_HEADROOM_CRS_CSR_REGr_HEADROOM_BMSK                                                               0x1ff
#define HWIO_RPMH_VRM_HEADROOM_CRS_CSR_REGr_HEADROOM_SHFT                                                                 0x0

#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOBr_ADDR(r)                                                                  (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000c004 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOBr_RMSK                                                                            0x1
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOBr_MAXr                                                                              7
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOBr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOBr_ADDR(r), HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOBr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOBr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOBr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOBr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOBr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOBr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOBr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOBr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOBr_ENABLE_BMSK                                                                     0x1
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOBr_ENABLE_SHFT                                                                     0x0

#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOr_ADDR(r)                                                                   (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000d804 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOr_RMSK                                                                             0x1
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOr_MAXr                                                                               0
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOr_ADDR(r), HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOr_ENABLE_BMSK                                                                      0x1
#define HWIO_RPMH_VRM_ENABLE_CRS_CSR_XOr_ENABLE_SHFT                                                                      0x0

#define HWIO_RPMH_VRM_VOLTAGE_WRS_CSR_REGr_ADDR(r)                                                                 (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000020 + 0x100 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_WRS_CSR_REGr_RMSK                                                                        0x1fff
#define HWIO_RPMH_VRM_VOLTAGE_WRS_CSR_REGr_MAXr                                                                            45
#define HWIO_RPMH_VRM_VOLTAGE_WRS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_WRS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_VOLTAGE_WRS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_WRS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_WRS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_WRS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_WRS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_VOLTAGE_WRS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_WRS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_VOLTAGE_WRS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_VOLTAGE_WRS_CSR_REGr_VOLTAGE_BMSK                                                                0x1fff
#define HWIO_RPMH_VRM_VOLTAGE_WRS_CSR_REGr_VOLTAGE_SHFT                                                                   0x0

#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_REGr_ADDR(r)                                                                  (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000024 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_REGr_RMSK                                                                            0x1
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_REGr_MAXr                                                                             45
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_WRS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_ENABLE_WRS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_WRS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_WRS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_WRS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_WRS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_REGr_ENABLE_BMSK                                                                     0x1
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_REGr_ENABLE_SHFT                                                                     0x0

#define HWIO_RPMH_VRM_MODE_WRS_CSR_REGr_ADDR(r)                                                                    (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000028 + 0x100 * (r))
#define HWIO_RPMH_VRM_MODE_WRS_CSR_REGr_RMSK                                                                              0x7
#define HWIO_RPMH_VRM_MODE_WRS_CSR_REGr_MAXr                                                                               45
#define HWIO_RPMH_VRM_MODE_WRS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_MODE_WRS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_MODE_WRS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_MODE_WRS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_MODE_WRS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_MODE_WRS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_MODE_WRS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_MODE_WRS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_WRS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_MODE_WRS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_MODE_WRS_CSR_REGr_MODE_BMSK                                                                         0x7
#define HWIO_RPMH_VRM_MODE_WRS_CSR_REGr_MODE_SHFT                                                                         0x0

#define HWIO_RPMH_VRM_HEADROOM_WRS_CSR_REGr_ADDR(r)                                                                (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000002c + 0x100 * (r))
#define HWIO_RPMH_VRM_HEADROOM_WRS_CSR_REGr_RMSK                                                                        0x1ff
#define HWIO_RPMH_VRM_HEADROOM_WRS_CSR_REGr_MAXr                                                                           45
#define HWIO_RPMH_VRM_HEADROOM_WRS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_WRS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_HEADROOM_WRS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_WRS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_WRS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_HEADROOM_WRS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_HEADROOM_WRS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_HEADROOM_WRS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_WRS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_HEADROOM_WRS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_HEADROOM_WRS_CSR_REGr_HEADROOM_BMSK                                                               0x1ff
#define HWIO_RPMH_VRM_HEADROOM_WRS_CSR_REGr_HEADROOM_SHFT                                                                 0x0

#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOBr_ADDR(r)                                                                  (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000c024 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOBr_RMSK                                                                            0x1
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOBr_MAXr                                                                              7
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOBr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOBr_ADDR(r), HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOBr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOBr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOBr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOBr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOBr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOBr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOBr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOBr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOBr_ENABLE_BMSK                                                                     0x1
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOBr_ENABLE_SHFT                                                                     0x0

#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOr_ADDR(r)                                                                   (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000d824 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOr_RMSK                                                                             0x1
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOr_MAXr                                                                               0
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOr_ADDR(r), HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOr_ENABLE_BMSK                                                                      0x1
#define HWIO_RPMH_VRM_ENABLE_WRS_CSR_XOr_ENABLE_SHFT                                                                      0x0

#define HWIO_RPMH_VRM_VOLTAGE_DRS_CSR_REGr_ADDR(r)                                                                 (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000040 + 0x100 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_DRS_CSR_REGr_RMSK                                                                        0x1fff
#define HWIO_RPMH_VRM_VOLTAGE_DRS_CSR_REGr_MAXr                                                                            45
#define HWIO_RPMH_VRM_VOLTAGE_DRS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DRS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_VOLTAGE_DRS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_DRS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DRS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_DRS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_DRS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_VOLTAGE_DRS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_DRS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_VOLTAGE_DRS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_VOLTAGE_DRS_CSR_REGr_VOLTAGE_BMSK                                                                0x1fff
#define HWIO_RPMH_VRM_VOLTAGE_DRS_CSR_REGr_VOLTAGE_SHFT                                                                   0x0

#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_REGr_ADDR(r)                                                                  (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000044 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_REGr_RMSK                                                                            0x1
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_REGr_MAXr                                                                             45
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DRS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_ENABLE_DRS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DRS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_DRS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_DRS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_DRS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_REGr_ENABLE_BMSK                                                                     0x1
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_REGr_ENABLE_SHFT                                                                     0x0

#define HWIO_RPMH_VRM_MODE_DRS_CSR_REGr_ADDR(r)                                                                    (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000048 + 0x100 * (r))
#define HWIO_RPMH_VRM_MODE_DRS_CSR_REGr_RMSK                                                                              0x7
#define HWIO_RPMH_VRM_MODE_DRS_CSR_REGr_MAXr                                                                               45
#define HWIO_RPMH_VRM_MODE_DRS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_MODE_DRS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_MODE_DRS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_MODE_DRS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_MODE_DRS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_MODE_DRS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_MODE_DRS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_MODE_DRS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_DRS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_MODE_DRS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_MODE_DRS_CSR_REGr_MODE_BMSK                                                                         0x7
#define HWIO_RPMH_VRM_MODE_DRS_CSR_REGr_MODE_SHFT                                                                         0x0

#define HWIO_RPMH_VRM_HEADROOM_DRS_CSR_REGr_ADDR(r)                                                                (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000004c + 0x100 * (r))
#define HWIO_RPMH_VRM_HEADROOM_DRS_CSR_REGr_RMSK                                                                        0x1ff
#define HWIO_RPMH_VRM_HEADROOM_DRS_CSR_REGr_MAXr                                                                           45
#define HWIO_RPMH_VRM_HEADROOM_DRS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_DRS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_HEADROOM_DRS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_DRS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_DRS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_HEADROOM_DRS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_HEADROOM_DRS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_HEADROOM_DRS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_DRS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_HEADROOM_DRS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_HEADROOM_DRS_CSR_REGr_HEADROOM_BMSK                                                               0x1ff
#define HWIO_RPMH_VRM_HEADROOM_DRS_CSR_REGr_HEADROOM_SHFT                                                                 0x0

#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOBr_ADDR(r)                                                                  (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000c044 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOBr_RMSK                                                                            0x1
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOBr_MAXr                                                                              7
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOBr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOBr_ADDR(r), HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOBr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOBr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOBr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOBr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOBr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOBr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOBr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOBr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOBr_ENABLE_BMSK                                                                     0x1
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOBr_ENABLE_SHFT                                                                     0x0

#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOr_ADDR(r)                                                                   (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000d844 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOr_RMSK                                                                             0x1
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOr_MAXr                                                                               0
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOr_ADDR(r), HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOr_ENABLE_BMSK                                                                      0x1
#define HWIO_RPMH_VRM_ENABLE_DRS_CSR_XOr_ENABLE_SHFT                                                                      0x0

#define HWIO_RPMH_VRM_VOLTAGE_ARS_CSR_REGr_ADDR(r)                                                                 (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000060 + 0x100 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_ARS_CSR_REGr_RMSK                                                                        0x1fff
#define HWIO_RPMH_VRM_VOLTAGE_ARS_CSR_REGr_MAXr                                                                            45
#define HWIO_RPMH_VRM_VOLTAGE_ARS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_ARS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_VOLTAGE_ARS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_ARS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_ARS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_ARS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_ARS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_VOLTAGE_ARS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_ARS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_VOLTAGE_ARS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_VOLTAGE_ARS_CSR_REGr_VOLTAGE_BMSK                                                                0x1fff
#define HWIO_RPMH_VRM_VOLTAGE_ARS_CSR_REGr_VOLTAGE_SHFT                                                                   0x0

#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_REGr_ADDR(r)                                                                  (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000064 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_REGr_RMSK                                                                            0x1
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_REGr_MAXr                                                                             45
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_ARS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_ENABLE_ARS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_ARS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_ARS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_ARS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_ARS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_REGr_ENABLE_BMSK                                                                     0x1
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_REGr_ENABLE_SHFT                                                                     0x0

#define HWIO_RPMH_VRM_MODE_ARS_CSR_REGr_ADDR(r)                                                                    (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000068 + 0x100 * (r))
#define HWIO_RPMH_VRM_MODE_ARS_CSR_REGr_RMSK                                                                              0x7
#define HWIO_RPMH_VRM_MODE_ARS_CSR_REGr_MAXr                                                                               45
#define HWIO_RPMH_VRM_MODE_ARS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_MODE_ARS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_MODE_ARS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_MODE_ARS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_MODE_ARS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_MODE_ARS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_MODE_ARS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_MODE_ARS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_ARS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_MODE_ARS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_MODE_ARS_CSR_REGr_MODE_BMSK                                                                         0x7
#define HWIO_RPMH_VRM_MODE_ARS_CSR_REGr_MODE_SHFT                                                                         0x0

#define HWIO_RPMH_VRM_HEADROOM_ARS_CSR_REGr_ADDR(r)                                                                (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000006c + 0x100 * (r))
#define HWIO_RPMH_VRM_HEADROOM_ARS_CSR_REGr_RMSK                                                                        0x1ff
#define HWIO_RPMH_VRM_HEADROOM_ARS_CSR_REGr_MAXr                                                                           45
#define HWIO_RPMH_VRM_HEADROOM_ARS_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_ARS_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_HEADROOM_ARS_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_ARS_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_ARS_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_HEADROOM_ARS_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_HEADROOM_ARS_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_HEADROOM_ARS_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_ARS_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_HEADROOM_ARS_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_HEADROOM_ARS_CSR_REGr_HEADROOM_BMSK                                                               0x1ff
#define HWIO_RPMH_VRM_HEADROOM_ARS_CSR_REGr_HEADROOM_SHFT                                                                 0x0

#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOBr_ADDR(r)                                                                  (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000c064 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOBr_RMSK                                                                            0x1
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOBr_MAXr                                                                              7
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOBr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOBr_ADDR(r), HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOBr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOBr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOBr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOBr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOBr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOBr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOBr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOBr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOBr_ENABLE_BMSK                                                                     0x1
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOBr_ENABLE_SHFT                                                                     0x0

#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOr_ADDR(r)                                                                   (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000d864 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOr_RMSK                                                                             0x1
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOr_MAXr                                                                               0
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOr_ADDR(r), HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOr_ENABLE_BMSK                                                                      0x1
#define HWIO_RPMH_VRM_ENABLE_ARS_CSR_XOr_ENABLE_SHFT                                                                      0x0

#define HWIO_RPMH_VRM_TYPE_CSR_REGr_ADDR(r)                                                                        (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000080 + 0x100 * (r))
#define HWIO_RPMH_VRM_TYPE_CSR_REGr_RMSK                                                                                  0xf
#define HWIO_RPMH_VRM_TYPE_CSR_REGr_MAXr                                                                                   45
#define HWIO_RPMH_VRM_TYPE_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_TYPE_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_TYPE_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_TYPE_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_TYPE_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_TYPE_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_TYPE_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_TYPE_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_TYPE_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_TYPE_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_TYPE_CSR_REGr_TYPE_BMSK                                                                             0xf
#define HWIO_RPMH_VRM_TYPE_CSR_REGr_TYPE_SHFT                                                                             0x0

#define HWIO_RPMH_VRM_BASE_ADDR_CSR_REGr_ADDR(r)                                                                   (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000084 + 0x100 * (r))
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_REGr_RMSK                                                                            0xff
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_REGr_MAXr                                                                              45
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_BASE_ADDR_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_BASE_ADDR_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_BASE_ADDR_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_BASE_ADDR_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_BASE_ADDR_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_BASE_ADDR_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_REGr_BASE_ADDR_BMSK                                                                  0xff
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_REGr_BASE_ADDR_SHFT                                                                   0x0

#define HWIO_RPMH_VRM_MAX_VOLTAGE_CSR_REGr_ADDR(r)                                                                 (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000088 + 0x100 * (r))
#define HWIO_RPMH_VRM_MAX_VOLTAGE_CSR_REGr_RMSK                                                                        0x1fff
#define HWIO_RPMH_VRM_MAX_VOLTAGE_CSR_REGr_MAXr                                                                            45
#define HWIO_RPMH_VRM_MAX_VOLTAGE_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_MAX_VOLTAGE_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_MAX_VOLTAGE_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_MAX_VOLTAGE_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_MAX_VOLTAGE_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_MAX_VOLTAGE_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MAX_VOLTAGE_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_MAX_VOLTAGE_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_MAX_VOLTAGE_CSR_REGr_MAX_VOLTAGE_BMSK                                                            0x1fff
#define HWIO_RPMH_VRM_MAX_VOLTAGE_CSR_REGr_MAX_VOLTAGE_SHFT                                                               0x0

#define HWIO_RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr_ADDR(r)                                                              (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000008c + 0x100 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr_RMSK                                                                        0x7
#define HWIO_RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr_MAXr                                                                         45
#define HWIO_RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr_VOLTAGE_SEQ_ID_BMSK                                                         0x7
#define HWIO_RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr_VOLTAGE_SEQ_ID_SHFT                                                         0x0

#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr_ADDR(r)                                                               (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000090 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr_RMSK                                                                         0x7
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr_MAXr                                                                          45
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr_ENABLE_SEQ_ID_BMSK                                                           0x7
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr_ENABLE_SEQ_ID_SHFT                                                           0x0

#define HWIO_RPMH_VRM_MODE_SEQ_ID_CSR_REGr_ADDR(r)                                                                 (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000094 + 0x100 * (r))
#define HWIO_RPMH_VRM_MODE_SEQ_ID_CSR_REGr_RMSK                                                                           0x7
#define HWIO_RPMH_VRM_MODE_SEQ_ID_CSR_REGr_MAXr                                                                            45
#define HWIO_RPMH_VRM_MODE_SEQ_ID_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_MODE_SEQ_ID_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_MODE_SEQ_ID_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_MODE_SEQ_ID_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_MODE_SEQ_ID_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_MODE_SEQ_ID_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_MODE_SEQ_ID_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_MODE_SEQ_ID_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_SEQ_ID_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_MODE_SEQ_ID_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_MODE_SEQ_ID_CSR_REGr_MODE_SEQ_ID_BMSK                                                               0x7
#define HWIO_RPMH_VRM_MODE_SEQ_ID_CSR_REGr_MODE_SEQ_ID_SHFT                                                               0x0

#define HWIO_RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr_ADDR(r)                                                             (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x00000098 + 0x100 * (r))
#define HWIO_RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr_RMSK                                                                       0x7
#define HWIO_RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr_MAXr                                                                        45
#define HWIO_RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr_HEADROOM_SEQ_ID_BMSK                                                       0x7
#define HWIO_RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr_HEADROOM_SEQ_ID_SHFT                                                       0x0

#define HWIO_RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr_ADDR(r)                                                          (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x000000a8 + 0x100 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr_RMSK                                                                    0x1
#define HWIO_RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr_MAXr                                                                     45
#define HWIO_RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr_VOLTAGE_DEP_ENABLE_BMSK                                                 0x1
#define HWIO_RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr_VOLTAGE_DEP_ENABLE_SHFT                                                 0x0

#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr_ADDR(r)                                                           (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x000000ac + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr_RMSK                                                                     0x1
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr_MAXr                                                                      45
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr_ENABLE_DEP_ENABLE_BMSK                                                   0x1
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr_ENABLE_DEP_ENABLE_SHFT                                                   0x0

#define HWIO_RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr_ADDR(r)                                                             (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x000000b0 + 0x100 * (r))
#define HWIO_RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr_RMSK                                                                       0x1
#define HWIO_RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr_MAXr                                                                        45
#define HWIO_RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr_MODE_DEP_ENABLE_BMSK                                                       0x1
#define HWIO_RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr_MODE_DEP_ENABLE_SHFT                                                       0x0

#define HWIO_RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr_ADDR(r)                                                         (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x000000b4 + 0x100 * (r))
#define HWIO_RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr_RMSK                                                                   0x1
#define HWIO_RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr_MAXr                                                                    45
#define HWIO_RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr_HEADROOM_DEP_ENABLE_BMSK                                               0x1
#define HWIO_RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr_HEADROOM_DEP_ENABLE_SHFT                                               0x0

#define HWIO_RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r)                                               (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x000000c4 + 0x100 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr_RMSK                                                         0x1
#define HWIO_RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr_MAXr                                                          45
#define HWIO_RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr_VOLTAGE_SETTLING_BLOCK_ENABLE_BMSK                           0x1
#define HWIO_RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr_VOLTAGE_SETTLING_BLOCK_ENABLE_SHFT                           0x0

#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r)                                                (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x000000c8 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_REGr_RMSK                                                          0x1
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_REGr_MAXr                                                           45
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_REGr_ENABLE_SETTLING_BLOCK_ENABLE_BMSK                             0x1
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_REGr_ENABLE_SETTLING_BLOCK_ENABLE_SHFT                             0x0

#define HWIO_RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r)                                                  (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x000000cc + 0x100 * (r))
#define HWIO_RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr_RMSK                                                            0x1
#define HWIO_RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr_MAXr                                                             45
#define HWIO_RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr_MODE_SETTLING_BLOCK_ENABLE_BMSK                                 0x1
#define HWIO_RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr_MODE_SETTLING_BLOCK_ENABLE_SHFT                                 0x0

#define HWIO_RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r)                                              (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x000000d0 + 0x100 * (r))
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr_RMSK                                                        0x1
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr_MAXr                                                         45
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr_HEADROOM_SETTLING_BLOCK_ENABLE_BMSK                         0x1
#define HWIO_RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr_HEADROOM_SETTLING_BLOCK_ENABLE_SHFT                         0x0

#define HWIO_RPMH_VRM_VREG_READY_ENABLE_CSR_REGr_ADDR(r)                                                           (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x000000d4 + 0x100 * (r))
#define HWIO_RPMH_VRM_VREG_READY_ENABLE_CSR_REGr_RMSK                                                                     0x1
#define HWIO_RPMH_VRM_VREG_READY_ENABLE_CSR_REGr_MAXr                                                                      45
#define HWIO_RPMH_VRM_VREG_READY_ENABLE_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_ENABLE_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_VREG_READY_ENABLE_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_VREG_READY_ENABLE_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_ENABLE_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VREG_READY_ENABLE_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VREG_READY_ENABLE_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_VREG_READY_ENABLE_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VREG_READY_ENABLE_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_VREG_READY_ENABLE_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_VREG_READY_ENABLE_CSR_REGr_VREG_READY_ENABLE_BMSK                                                   0x1
#define HWIO_RPMH_VRM_VREG_READY_ENABLE_CSR_REGr_VREG_READY_ENABLE_SHFT                                                   0x0

#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr_ADDR(r)                                                (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x000000d8 + 0x100 * (r))
#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr_RMSK                                                          0x1
#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr_MAXr                                                           45
#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr_VREG_READY_WDOG_ERROR_ENABLE_BMSK                             0x1
#define HWIO_RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr_VREG_READY_WDOG_ERROR_ENABLE_SHFT                             0x0

#define HWIO_RPMH_VRM_TYPE_CSR_XOBr_ADDR(r)                                                                        (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000c080 + 0x100 * (r))
#define HWIO_RPMH_VRM_TYPE_CSR_XOBr_RMSK                                                                                  0xf
#define HWIO_RPMH_VRM_TYPE_CSR_XOBr_MAXr                                                                                    7
#define HWIO_RPMH_VRM_TYPE_CSR_XOBr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_TYPE_CSR_XOBr_ADDR(r), HWIO_RPMH_VRM_TYPE_CSR_XOBr_RMSK)
#define HWIO_RPMH_VRM_TYPE_CSR_XOBr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_TYPE_CSR_XOBr_ADDR(r), mask)
#define HWIO_RPMH_VRM_TYPE_CSR_XOBr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_TYPE_CSR_XOBr_ADDR(r),val)
#define HWIO_RPMH_VRM_TYPE_CSR_XOBr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_TYPE_CSR_XOBr_ADDR(r),mask,val,HWIO_RPMH_VRM_TYPE_CSR_XOBr_INI(r))
#define HWIO_RPMH_VRM_TYPE_CSR_XOBr_TYPE_BMSK                                                                             0xf
#define HWIO_RPMH_VRM_TYPE_CSR_XOBr_TYPE_SHFT                                                                             0x0

#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOBr_ADDR(r)                                                                   (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000c084 + 0x100 * (r))
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOBr_RMSK                                                                            0xff
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOBr_MAXr                                                                               7
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOBr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_BASE_ADDR_CSR_XOBr_ADDR(r), HWIO_RPMH_VRM_BASE_ADDR_CSR_XOBr_RMSK)
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOBr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_BASE_ADDR_CSR_XOBr_ADDR(r), mask)
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOBr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_BASE_ADDR_CSR_XOBr_ADDR(r),val)
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOBr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_BASE_ADDR_CSR_XOBr_ADDR(r),mask,val,HWIO_RPMH_VRM_BASE_ADDR_CSR_XOBr_INI(r))
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOBr_BASE_ADDR_BMSK                                                                  0xff
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOBr_BASE_ADDR_SHFT                                                                   0x0

#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr_ADDR(r)                                                               (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000c090 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr_RMSK                                                                         0x7
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr_MAXr                                                                           7
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr_ADDR(r), HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr_ENABLE_SEQ_ID_BMSK                                                           0x7
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr_ENABLE_SEQ_ID_SHFT                                                           0x0

#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr_ADDR(r)                                                           (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000c0ac + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr_RMSK                                                                     0x1
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr_MAXr                                                                       7
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr_ADDR(r), HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr_ENABLE_DEP_ENABLE_BMSK                                                   0x1
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr_ENABLE_DEP_ENABLE_SHFT                                                   0x0

#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOBr_ADDR(r)                                                (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000c0c8 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOBr_RMSK                                                          0x1
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOBr_MAXr                                                            7
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOBr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOBr_ADDR(r), HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOBr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOBr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOBr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOBr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOBr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOBr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOBr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOBr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOBr_ENABLE_SETTLING_BLOCK_ENABLE_BMSK                             0x1
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOBr_ENABLE_SETTLING_BLOCK_ENABLE_SHFT                             0x0

#define HWIO_RPMH_VRM_TYPE_CSR_XOr_ADDR(r)                                                                         (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000c880 + 0x100 * (r))
#define HWIO_RPMH_VRM_TYPE_CSR_XOr_RMSK                                                                                   0xf
#define HWIO_RPMH_VRM_TYPE_CSR_XOr_MAXr                                                                                     0
#define HWIO_RPMH_VRM_TYPE_CSR_XOr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_TYPE_CSR_XOr_ADDR(r), HWIO_RPMH_VRM_TYPE_CSR_XOr_RMSK)
#define HWIO_RPMH_VRM_TYPE_CSR_XOr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_TYPE_CSR_XOr_ADDR(r), mask)
#define HWIO_RPMH_VRM_TYPE_CSR_XOr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_TYPE_CSR_XOr_ADDR(r),val)
#define HWIO_RPMH_VRM_TYPE_CSR_XOr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_TYPE_CSR_XOr_ADDR(r),mask,val,HWIO_RPMH_VRM_TYPE_CSR_XOr_INI(r))
#define HWIO_RPMH_VRM_TYPE_CSR_XOr_TYPE_BMSK                                                                              0xf
#define HWIO_RPMH_VRM_TYPE_CSR_XOr_TYPE_SHFT                                                                              0x0

#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOr_ADDR(r)                                                                    (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000c884 + 0x100 * (r))
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOr_RMSK                                                                             0xff
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOr_MAXr                                                                                0
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_BASE_ADDR_CSR_XOr_ADDR(r), HWIO_RPMH_VRM_BASE_ADDR_CSR_XOr_RMSK)
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_BASE_ADDR_CSR_XOr_ADDR(r), mask)
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_BASE_ADDR_CSR_XOr_ADDR(r),val)
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_BASE_ADDR_CSR_XOr_ADDR(r),mask,val,HWIO_RPMH_VRM_BASE_ADDR_CSR_XOr_INI(r))
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOr_BASE_ADDR_BMSK                                                                   0xff
#define HWIO_RPMH_VRM_BASE_ADDR_CSR_XOr_BASE_ADDR_SHFT                                                                    0x0

#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOr_ADDR(r)                                                                (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000c890 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOr_RMSK                                                                          0x7
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOr_MAXr                                                                            0
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOr_ADDR(r), HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOr_ENABLE_SEQ_ID_BMSK                                                            0x7
#define HWIO_RPMH_VRM_ENABLE_SEQ_ID_CSR_XOr_ENABLE_SEQ_ID_SHFT                                                            0x0

#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOr_ADDR(r)                                                            (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000c8ac + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOr_RMSK                                                                      0x1
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOr_MAXr                                                                        0
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOr_ADDR(r), HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOr_ENABLE_DEP_ENABLE_BMSK                                                    0x1
#define HWIO_RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOr_ENABLE_DEP_ENABLE_SHFT                                                    0x0

#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOr_ADDR(r)                                                 (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000c8c8 + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOr_RMSK                                                           0x1
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOr_MAXr                                                             0
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOr_ADDR(r), HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOr_ADDR(r), mask)
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOr_ADDR(r),val)
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOr_ADDR(r),mask,val,HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOr_INI(r))
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOr_ENABLE_SETTLING_BLOCK_ENABLE_BMSK                              0x1
#define HWIO_RPMH_VRM_ENABLE_SETTLING_BLOCK_ENABLE_CSR_XOr_ENABLE_SETTLING_BLOCK_ENABLE_SHFT                              0x0

#define HWIO_RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr_ADDR(r)                                          (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000c8e4 + 0x100 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr_RMSK                                                    0x1
#define HWIO_RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr_MAXr                                                     45
#define HWIO_RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_BMSK                 0x1
#define HWIO_RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_SHFT                 0x0

#define HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_ADDR(r)                                                               (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x000000fc + 0x100 * (r))
#define HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_RMSK                                                                         0x7
#define HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_MAXr                                                                          45
#define HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_ADDR(r), HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_RMSK)
#define HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_ADDR(r), mask)
#define HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_ADDR(r),val)
#define HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_ADDR(r),mask,val,HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_INI(r))
#define HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_MODE_BMSK                                                                    0x4
#define HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_MODE_SHFT                                                                    0x2
#define HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_ENABLE_BMSK                                                                  0x2
#define HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_ENABLE_SHFT                                                                  0x1
#define HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_VOLTAGE_BMSK                                                                 0x1
#define HWIO_RPMH_VRM_SW_WORKAROUND_CSR_REGr_VOLTAGE_SHFT                                                                 0x0

#define HWIO_RPMH_VRM_XO_SETTLING_TIME_CSR_RESr_ADDR(r)                                                            (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000f000 + 0x4 * (r))
#define HWIO_RPMH_VRM_XO_SETTLING_TIME_CSR_RESr_RMSK                                                                   0xffff
#define HWIO_RPMH_VRM_XO_SETTLING_TIME_CSR_RESr_MAXr                                                                        0
#define HWIO_RPMH_VRM_XO_SETTLING_TIME_CSR_RESr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_XO_SETTLING_TIME_CSR_RESr_ADDR(r), HWIO_RPMH_VRM_XO_SETTLING_TIME_CSR_RESr_RMSK)
#define HWIO_RPMH_VRM_XO_SETTLING_TIME_CSR_RESr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_XO_SETTLING_TIME_CSR_RESr_ADDR(r), mask)
#define HWIO_RPMH_VRM_XO_SETTLING_TIME_CSR_RESr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_XO_SETTLING_TIME_CSR_RESr_ADDR(r),val)
#define HWIO_RPMH_VRM_XO_SETTLING_TIME_CSR_RESr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_XO_SETTLING_TIME_CSR_RESr_ADDR(r),mask,val,HWIO_RPMH_VRM_XO_SETTLING_TIME_CSR_RESr_INI(r))
#define HWIO_RPMH_VRM_XO_SETTLING_TIME_CSR_RESr_XO_SETTLING_TIME_BMSK                                                  0xffff
#define HWIO_RPMH_VRM_XO_SETTLING_TIME_CSR_RESr_XO_SETTLING_TIME_SHFT                                                     0x0

#define HWIO_RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr_ADDR(r)                                                           (RPMH_VRM_VRM_RES_CFG_REG_BASE      + 0x0000f108 + 0x4 * (r))
#define HWIO_RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr_RMSK                                                                  0xffff
#define HWIO_RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr_MAXr                                                                       7
#define HWIO_RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr_ADDR(r), HWIO_RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr_RMSK)
#define HWIO_RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr_ADDR(r), mask)
#define HWIO_RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr_ADDR(r),val)
#define HWIO_RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr_ADDR(r),mask,val,HWIO_RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr_INI(r))
#define HWIO_RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr_XOB_SETTLING_TIME_BMSK                                                0xffff
#define HWIO_RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr_XOB_SETTLING_TIME_SHFT                                                   0x0


#endif /* __VRM_RESCFG_HWIO_H__ */
