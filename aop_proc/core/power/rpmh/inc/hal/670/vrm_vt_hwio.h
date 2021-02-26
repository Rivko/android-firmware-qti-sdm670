#ifndef __VRM_VT_HWIO_H__
#define __VRM_VT_HWIO_H__
/*
===========================================================================
*/
/**
  @file vrm_vt_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r105_MTO]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_VRM_VRM_VOTETABLE


  Generation parameters: 
  { u'filename': u'vrm_vt_hwio.h',
    u'header': u'#include "msmhwiobase.h"',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'RPMH_VRM_VRM_VOTETABLE']}
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/rpmh/inc/hal/670/vrm_vt_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: RPMH_VRM_VRM_VOTETABLE
 *--------------------------------------------------------------------------*/

#define RPMH_VRM_VRM_VOTETABLE_REG_BASE                              (AOSS_BASE      + 0x00800000)

#define HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_ADDR(d,r)               (RPMH_VRM_VRM_VOTETABLE_REG_BASE      + 0x00000000 + 0x10000 * (d) + 0x100 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_RMSK                    0x80001fff
#define HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_MAXd                            13
#define HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_MAXr                            45
#define HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_INI2(d,r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_ADDR(d,r), HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_INMI2(d,r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_ADDR(d,r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_OUTI2(d,r,val)    \
        out_dword(HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_ADDR(d,r),val)
#define HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_OUTMI2(d,r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_ADDR(d,r),mask,val,HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_INI2(d,r))
#define HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_RESP_REQ_BMSK           0x80000000
#define HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_RESP_REQ_SHFT                 0x1f
#define HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_VOLTAGE_BMSK                0x1fff
#define HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_VOLTAGE_SHFT                   0x0

#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_ADDR(d,r)                (RPMH_VRM_VRM_VOTETABLE_REG_BASE      + 0x00000004 + 0x10000 * (d) + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_RMSK                     0x80000001
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_MAXd                             13
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_MAXr                             45
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_INI2(d,r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_ADDR(d,r), HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_INMI2(d,r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_ADDR(d,r), mask)
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_OUTI2(d,r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_ADDR(d,r),val)
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_OUTMI2(d,r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_ADDR(d,r),mask,val,HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_INI2(d,r))
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_RESP_REQ_BMSK            0x80000000
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_RESP_REQ_SHFT                  0x1f
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_ENABLE_BMSK                     0x1
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_ENABLE_SHFT                     0x0

#define HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_ADDR(d,r)                  (RPMH_VRM_VRM_VOTETABLE_REG_BASE      + 0x00000008 + 0x10000 * (d) + 0x100 * (r))
#define HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_RMSK                       0x80000007
#define HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_MAXd                               13
#define HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_MAXr                               45
#define HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_INI2(d,r)        \
        in_dword_masked(HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_ADDR(d,r), HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_RMSK)
#define HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_INMI2(d,r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_ADDR(d,r), mask)
#define HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_OUTI2(d,r,val)    \
        out_dword(HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_ADDR(d,r),val)
#define HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_OUTMI2(d,r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_ADDR(d,r),mask,val,HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_INI2(d,r))
#define HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_RESP_REQ_BMSK              0x80000000
#define HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_RESP_REQ_SHFT                    0x1f
#define HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_MODE_BMSK                         0x7
#define HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_MODE_SHFT                         0x0

#define HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_ADDR(d,r)              (RPMH_VRM_VRM_VOTETABLE_REG_BASE      + 0x0000000c + 0x10000 * (d) + 0x100 * (r))
#define HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_RMSK                   0x800001ff
#define HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_MAXd                           13
#define HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_MAXr                           45
#define HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_INI2(d,r)        \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_ADDR(d,r), HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_INMI2(d,r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_ADDR(d,r), mask)
#define HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_OUTI2(d,r,val)    \
        out_dword(HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_ADDR(d,r),val)
#define HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_OUTMI2(d,r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_ADDR(d,r),mask,val,HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_INI2(d,r))
#define HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_RESP_REQ_BMSK          0x80000000
#define HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_RESP_REQ_SHFT                0x1f
#define HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_HEADROOM_BMSK               0x1ff
#define HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_HEADROOM_SHFT                 0x0

#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_ADDR(d,r)                (RPMH_VRM_VRM_VOTETABLE_REG_BASE      + 0x0000c004 + 0x10000 * (d) + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_RMSK                     0x80000001
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_MAXd                             13
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_MAXr                              7
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_INI2(d,r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_ADDR(d,r), HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_INMI2(d,r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_ADDR(d,r), mask)
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_OUTI2(d,r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_ADDR(d,r),val)
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_OUTMI2(d,r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_ADDR(d,r),mask,val,HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_INI2(d,r))
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_RESP_REQ_BMSK            0x80000000
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_RESP_REQ_SHFT                  0x1f
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_ENABLE_BMSK                     0x1
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_ENABLE_SHFT                     0x0

#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_ADDR(d,r)                 (RPMH_VRM_VRM_VOTETABLE_REG_BASE      + 0x0000d804 + 0x10000 * (d) + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_RMSK                      0x80000001
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_MAXd                              13
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_MAXr                               0
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_INI2(d,r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_ADDR(d,r), HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_INMI2(d,r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_ADDR(d,r), mask)
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_OUTI2(d,r,val)    \
        out_dword(HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_ADDR(d,r),val)
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_OUTMI2(d,r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_ADDR(d,r),mask,val,HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_INI2(d,r))
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_RESP_REQ_BMSK             0x80000000
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_RESP_REQ_SHFT                   0x1f
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_ENABLE_BMSK                      0x1
#define HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOr_ENABLE_SHFT                      0x0

#define HWIO_RPMH_VRM_VOLTAGE_CRS_DRVd_REGr_ADDR(d,r)                (RPMH_VRM_VRM_VOTETABLE_REG_BASE      + 0x00000020 + 0x10000 * (d) + 0x100 * (r))
#define HWIO_RPMH_VRM_VOLTAGE_CRS_DRVd_REGr_RMSK                         0x1fff
#define HWIO_RPMH_VRM_VOLTAGE_CRS_DRVd_REGr_MAXd                             13
#define HWIO_RPMH_VRM_VOLTAGE_CRS_DRVd_REGr_MAXr                             45
#define HWIO_RPMH_VRM_VOLTAGE_CRS_DRVd_REGr_INI2(d,r)        \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_CRS_DRVd_REGr_ADDR(d,r), HWIO_RPMH_VRM_VOLTAGE_CRS_DRVd_REGr_RMSK)
#define HWIO_RPMH_VRM_VOLTAGE_CRS_DRVd_REGr_INMI2(d,r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_VOLTAGE_CRS_DRVd_REGr_ADDR(d,r), mask)
#define HWIO_RPMH_VRM_VOLTAGE_CRS_DRVd_REGr_VOLTAGE_BMSK                 0x1fff
#define HWIO_RPMH_VRM_VOLTAGE_CRS_DRVd_REGr_VOLTAGE_SHFT                    0x0

#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_REGr_ADDR(d,r)                 (RPMH_VRM_VRM_VOTETABLE_REG_BASE      + 0x00000024 + 0x10000 * (d) + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_REGr_RMSK                             0x1
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_REGr_MAXd                              13
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_REGr_MAXr                              45
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_REGr_INI2(d,r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CRS_DRVd_REGr_ADDR(d,r), HWIO_RPMH_VRM_ENABLE_CRS_DRVd_REGr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_REGr_INMI2(d,r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CRS_DRVd_REGr_ADDR(d,r), mask)
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_REGr_ENABLE_BMSK                      0x1
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_REGr_ENABLE_SHFT                      0x0

#define HWIO_RPMH_VRM_MODE_CRS_DRVd_REGr_ADDR(d,r)                   (RPMH_VRM_VRM_VOTETABLE_REG_BASE      + 0x00000028 + 0x10000 * (d) + 0x100 * (r))
#define HWIO_RPMH_VRM_MODE_CRS_DRVd_REGr_RMSK                               0x7
#define HWIO_RPMH_VRM_MODE_CRS_DRVd_REGr_MAXd                                13
#define HWIO_RPMH_VRM_MODE_CRS_DRVd_REGr_MAXr                                45
#define HWIO_RPMH_VRM_MODE_CRS_DRVd_REGr_INI2(d,r)        \
        in_dword_masked(HWIO_RPMH_VRM_MODE_CRS_DRVd_REGr_ADDR(d,r), HWIO_RPMH_VRM_MODE_CRS_DRVd_REGr_RMSK)
#define HWIO_RPMH_VRM_MODE_CRS_DRVd_REGr_INMI2(d,r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_MODE_CRS_DRVd_REGr_ADDR(d,r), mask)
#define HWIO_RPMH_VRM_MODE_CRS_DRVd_REGr_MODE_BMSK                          0x7
#define HWIO_RPMH_VRM_MODE_CRS_DRVd_REGr_MODE_SHFT                          0x0

#define HWIO_RPMH_VRM_HEADROOM_CRS_DRVd_REGr_ADDR(d,r)               (RPMH_VRM_VRM_VOTETABLE_REG_BASE      + 0x0000002c + 0x10000 * (d) + 0x100 * (r))
#define HWIO_RPMH_VRM_HEADROOM_CRS_DRVd_REGr_RMSK                         0x1ff
#define HWIO_RPMH_VRM_HEADROOM_CRS_DRVd_REGr_MAXd                            13
#define HWIO_RPMH_VRM_HEADROOM_CRS_DRVd_REGr_MAXr                            45
#define HWIO_RPMH_VRM_HEADROOM_CRS_DRVd_REGr_INI2(d,r)        \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_CRS_DRVd_REGr_ADDR(d,r), HWIO_RPMH_VRM_HEADROOM_CRS_DRVd_REGr_RMSK)
#define HWIO_RPMH_VRM_HEADROOM_CRS_DRVd_REGr_INMI2(d,r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_HEADROOM_CRS_DRVd_REGr_ADDR(d,r), mask)
#define HWIO_RPMH_VRM_HEADROOM_CRS_DRVd_REGr_HEADROOM_BMSK                0x1ff
#define HWIO_RPMH_VRM_HEADROOM_CRS_DRVd_REGr_HEADROOM_SHFT                  0x0

#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOBr_ADDR(d,r)                 (RPMH_VRM_VRM_VOTETABLE_REG_BASE      + 0x0000c024 + 0x10000 * (d) + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOBr_RMSK                             0x1
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOBr_MAXd                              13
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOBr_MAXr                               7
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOBr_INI2(d,r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOBr_ADDR(d,r), HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOBr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOBr_INMI2(d,r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOBr_ADDR(d,r), mask)
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOBr_ENABLE_BMSK                      0x1
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOBr_ENABLE_SHFT                      0x0

#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOr_ADDR(d,r)                  (RPMH_VRM_VRM_VOTETABLE_REG_BASE      + 0x0000d824 + 0x10000 * (d) + 0x100 * (r))
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOr_RMSK                              0x1
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOr_MAXd                               13
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOr_MAXr                                0
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOr_INI2(d,r)        \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOr_ADDR(d,r), HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOr_RMSK)
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOr_INMI2(d,r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOr_ADDR(d,r), mask)
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOr_ENABLE_BMSK                       0x1
#define HWIO_RPMH_VRM_ENABLE_CRS_DRVd_XOr_ENABLE_SHFT                       0x0


#endif /* __VRM_VT_HWIO_H__ */
