#ifndef __BCM_VT_HWIO_H__
#define __BCM_VT_HWIO_H__
/*
===========================================================================
*/
/**
  @file bcm_vt_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r105_MTO]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_BCM_BCM_VOTETABLE


  Generation parameters: 
  { u'filename': u'bcm_vt_hwio.h',
    u'header': u'#include "msmhwiobase.h"',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'RPMH_BCM_BCM_VOTETABLE']}
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/rpmh/inc/hal/670/bcm_vt_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: RPMH_BCM_BCM_VOTETABLE
 *--------------------------------------------------------------------------*/

#define RPMH_BCM_BCM_VOTETABLE_REG_BASE                                                     (AOSS_BASE      + 0x00a00000)

#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_ADDR(r,d)                                          (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x00000000 + 0x10000 * (r) + 0x4 * (d))
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_RMSK                                               0xefffffff
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_MAXr                                                       11
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_MAXd                                                       63
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_INI2(r,d)        \
        in_dword_masked(HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_ADDR(r,d), HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_RMSK)
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_INMI2(r,d,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_ADDR(r,d), mask)
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_OUTI2(r,d,val)    \
        out_dword(HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_ADDR(r,d),val)
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_OUTMI2(r,d,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_ADDR(r,d),mask,val,HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_INI2(r,d))
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_RESP_REQ_BMSK                                      0x80000000
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_RESP_REQ_SHFT                                            0x1f
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_COMMIT_BMSK                                        0x40000000
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_COMMIT_SHFT                                              0x1e
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_VOTE_VALID_BMSK                                    0x20000000
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_VOTE_VALID_SHFT                                          0x1d
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_VOTE_X_BMSK                                         0xfffc000
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_VOTE_X_SHFT                                               0xe
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_VOTE_Y_BMSK                                            0x3fff
#define HWIO_RPMH_BCM_VOTETABLE_DRVr_NDd_VOTE_Y_SHFT                                               0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_AGG_FSM_ADDR(r)                                 (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x00000200 + 0x10000 * (r))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_AGG_FSM_RMSK                                        0xffff
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_AGG_FSM_MAXr                                            11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_AGG_FSM_INI(r)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_AGG_FSM_ADDR(r), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_AGG_FSM_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_AGG_FSM_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_AGG_FSM_ADDR(r), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_AGG_FSM_AGG_BUSY_CD_BMSK                            0xffff
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_AGG_FSM_AGG_BUSY_CD_SHFT                               0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_ADDR(r,m)                                   (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x00000204 + 0x10000 * (r) + 0x4 * (m))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_RMSK                                        0x7fff10ff
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_MAXr                                                11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_MAXm                                                15
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_INI2(r,m)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_ADDR(r,m), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_INMI2(r,m,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_ADDR(r,m), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_AGG_BW_BMSK                                 0x7fff0000
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_AGG_BW_SHFT                                       0x10
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_COMMIT_CD_RAW_BMSK                              0x1000
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_COMMIT_CD_RAW_SHFT                                 0xc
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_FINAL_CP_BMSK                                     0xf0
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_FINAL_CP_SHFT                                      0x4
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_AGG_CP_BMSK                                        0xf
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_FE_CDm_AGG_CP_SHFT                                        0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_0_ADDR(r)                                 (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x00000284 + 0x10000 * (r))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_0_RMSK                                    0xffffffff
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_0_MAXr                                            11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_0_INI(r)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_0_ADDR(r), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_0_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_0_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_0_ADDR(r), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_0_DIRTY_ND_31_0_BMSK                      0xffffffff
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_0_DIRTY_ND_31_0_SHFT                             0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_1_ADDR(r)                                 (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x00000288 + 0x10000 * (r))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_1_RMSK                                    0xffffffff
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_1_MAXr                                            11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_1_INI(r)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_1_ADDR(r), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_1_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_1_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_1_ADDR(r), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_1_DIRTY_ND_63_32_BMSK                     0xffffffff
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_DIRTY_ND_1_DIRTY_ND_63_32_SHFT                            0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_SNDs_ADDR(r,s)                                     (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x0000028c + 0x10000 * (r) + 0x4 * (s))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_SNDs_RMSK                                                 0xf
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_SNDs_MAXr                                                  11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_SNDs_MAXs                                                   7
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_SNDs_INI2(r,s)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_SNDs_ADDR(r,s), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_SNDs_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_SNDs_INMI2(r,s,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_SNDs_ADDR(r,s), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_SNDs_SEL_CP_BMSK                                          0xf
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_SNDs_SEL_CP_SHFT                                          0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_NDd_ADDR(r,d)                               (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x000002ac + 0x10000 * (r) + 0x4 * (d))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_NDd_RMSK                                        0x7fff
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_NDd_MAXr                                            11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_NDd_MAXd                                            63
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_NDd_INI2(r,d)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_NDd_ADDR(r,d), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_NDd_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_NDd_INMI2(r,d,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_NDd_ADDR(r,d), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_NDd_AGG_BW_BMSK                                 0x7fff
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_NDd_AGG_BW_SHFT                                    0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_ADDR(r,s)                             (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x000004ac + 0x10000 * (r) + 0x4 * (s))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_RMSK                                      0x7fff
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_MAXr                                          11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_MAXs                                           7
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_INI2(r,s)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_ADDR(r,s), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_INMI2(r,s,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_ADDR(r,s), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_AGG_BW_Z_BMSK                             0x7fff
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_AGG_BW_Z_SHFT                                0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_ADDR(r,m)                                   (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x000004cc + 0x10000 * (r) + 0x4 * (m))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_RMSK                                           0xfffff
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_MAXr                                                11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_MAXm                                                15
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_INI2(r,m)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_ADDR(r,m), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_INMI2(r,m,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_ADDR(r,m), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_CLK_DEST_STATE_BMSK                            0xf0000
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_CLK_DEST_STATE_SHFT                               0x10
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_COMBINED_CP_BMSK                                0xf000
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_COMBINED_CP_SHFT                                   0xc
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_SW_CP_SNAP_BMSK                                  0xf00
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_SW_CP_SNAP_SHFT                                    0x8
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_WRITTEN_CP_BMSK                                   0xf0
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_WRITTEN_CP_SHFT                                    0x4
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_CURR_CP_BMSK                                       0xf
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CDm_CURR_CP_SHFT                                       0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_SEQ_CDm_ADDR(r,m)                               (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x0000054c + 0x10000 * (r) + 0x4 * (m))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_SEQ_CDm_RMSK                                        0xff01
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_SEQ_CDm_MAXr                                            11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_SEQ_CDm_MAXm                                            15
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_SEQ_CDm_INI2(r,m)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_SEQ_CDm_ADDR(r,m), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_SEQ_CDm_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_SEQ_CDm_INMI2(r,m,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_SEQ_CDm_ADDR(r,m), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_SEQ_CDm_SEQ_CURR_PC_BMSK                            0xff00
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_SEQ_CDm_SEQ_CURR_PC_SHFT                               0x8
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_SEQ_CDm_SEQ_STATE_BMSK                                 0x1
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_SEQ_CDm_SEQ_STATE_SHFT                                 0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_ADDR(r,m)                  (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x000005cc + 0x10000 * (r) + 0x4 * (m))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_RMSK                              0x7
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_MAXr                               11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_MAXm                               15
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_INI2(r,m)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_ADDR(r,m), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_INMI2(r,m,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_ADDR(r,m), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_ISSUE_STATUS_BMSK                 0x7
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_ISSUE_STATUS_SHFT                 0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_ADDR(r,m)                   (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x0000064c + 0x10000 * (r) + 0x4 * (m))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_RMSK                               0x7
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_MAXr                                11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_MAXm                                15
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_INI2(r,m)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_ADDR(r,m), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_INMI2(r,m,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_ADDR(r,m), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_SENT_STATUS_BMSK                   0x7
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_SENT_STATUS_SHFT                   0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_ADDR(r,m)                   (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x000006cc + 0x10000 * (r) + 0x4 * (m))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_RMSK                               0x7
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_MAXr                                11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_MAXm                                15
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_INI2(r,m)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_ADDR(r,m), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_INMI2(r,m,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_ADDR(r,m), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_CMPL_STATUS_BMSK                   0x7
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_CMPL_STATUS_SHFT                   0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_ADDR(r,m)                        (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x0000074c + 0x10000 * (r) + 0x4 * (m))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_RMSK                                    0x7
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_MAXr                                     11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_MAXm                                     15
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_INI2(r,m)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_ADDR(r,m), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_INMI2(r,m,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_ADDR(r,m), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_TCS_CURR_STATE_BMSK                     0x7
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_TCS_CURR_STATE_SHFT                     0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_ADDR(r)                               (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x0000085c + 0x10000 * (r))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_RMSK                                     0xfff01
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_MAXr                                          11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_INI(r)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_ADDR(r), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_ADDR(r), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_CTT_EMPTY_DRV_BMSK                       0xfff00
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_CTT_EMPTY_DRV_SHFT                           0x8
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_CTT_EMPTY_BMSK                               0x1
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_CTT_EMPTY_SHFT                               0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BCM_ADDR(r)                                        (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x00000860 + 0x10000 * (r))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BCM_RMSK                                                  0x1
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BCM_MAXr                                                   11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BCM_INI(r)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BCM_ADDR(r), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BCM_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BCM_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BCM_ADDR(r), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BCM_BCM_IDLE_BMSK                                         0x1
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_BCM_BCM_IDLE_SHFT                                         0x0

#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_HW_VERSION_ADDR(r)                                 (RPMH_BCM_BCM_VOTETABLE_REG_BASE      + 0x00000864 + 0x10000 * (r))
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_HW_VERSION_RMSK                                      0xffffff
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_HW_VERSION_MAXr                                            11
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_HW_VERSION_INI(r)        \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_HW_VERSION_ADDR(r), HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_HW_VERSION_RMSK)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_HW_VERSION_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_HW_VERSION_ADDR(r), mask)
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_HW_VERSION_MAJOR_BMSK                                0xff0000
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_HW_VERSION_MAJOR_SHFT                                    0x10
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_HW_VERSION_MINOR_BMSK                                  0xff00
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_HW_VERSION_MINOR_SHFT                                     0x8
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_HW_VERSION_STEP_BMSK                                     0xff
#define HWIO_RPMH_BCM_SHADOW_STATUS_DRVr_HW_VERSION_STEP_SHFT                                      0x0


#endif /* __BCM_VT_HWIO_H__ */
