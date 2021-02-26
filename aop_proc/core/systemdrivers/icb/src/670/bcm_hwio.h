#ifndef __BCM_HWIO_H__
#define __BCM_HWIO_H__
/*
===========================================================================
*/
/**
  @file bcm_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    .*RPMH_BCM.*

  'Include' filters applied: <none>
  'Exclude' filters applied: RESERVED DUMMY CD0_CPn[RPMH] CD1_CPn[RPMH] CD2_CPn[RPMH] CD3_CPn[RPMH] CD4_CPn[RPMH] CD5_CPn[RPMH] CD6_CPn[RPMH] CD7_CPn[RPMH] CD8_CPn[RPMH] CD9_CPn[RPMH] CD10_CPn[RPMH] CD11_CPn[RPMH] CD12_CPn[RPMH] CD13_CPn[RPMH] CD14_CPn[RPMH] CD15_CPn[RPMH] 
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/systemdrivers/icb/src/670/bcm_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/


#include <HALhwio.h>
#include <msmhwiobase.h>


/*----------------------------------------------------------------------------
 * MODULE: RPMH_BCM_BCM_TOP
 *--------------------------------------------------------------------------*/

#define RPMH_BCM_BCM_TOP_REG_BASE (AOSS_BASE      + 0x00a00000)

/*----------------------------------------------------------------------------
 * MODULE: BCM_VOTETABLE
 *--------------------------------------------------------------------------*/

#define BCM_VOTETABLE_REG_BASE                                                              (AOSS_BASE      + 0x00a00000)

#define HWIO_VOTETABLE_DRVr_NDd_ADDR(r,d)                                                   (BCM_VOTETABLE_REG_BASE      + 0x00000000 + 0x10000 * (r) + 0x4 * (d))
#define HWIO_VOTETABLE_DRVr_NDd_RMSK                                                        0xefffffff
#define HWIO_VOTETABLE_DRVr_NDd_MAXr                                                                11
#define HWIO_VOTETABLE_DRVr_NDd_MAXd                                                                63
#define HWIO_VOTETABLE_DRVr_NDd_INI2(r,d)        \
        in_dword_masked(HWIO_VOTETABLE_DRVr_NDd_ADDR(r,d), HWIO_VOTETABLE_DRVr_NDd_RMSK)
#define HWIO_VOTETABLE_DRVr_NDd_INMI2(r,d,mask)    \
        in_dword_masked(HWIO_VOTETABLE_DRVr_NDd_ADDR(r,d), mask)
#define HWIO_VOTETABLE_DRVr_NDd_OUTI2(r,d,val)    \
        out_dword(HWIO_VOTETABLE_DRVr_NDd_ADDR(r,d),val)
#define HWIO_VOTETABLE_DRVr_NDd_OUTMI2(r,d,mask,val) \
        out_dword_masked_ns(HWIO_VOTETABLE_DRVr_NDd_ADDR(r,d),mask,val,HWIO_VOTETABLE_DRVr_NDd_INI2(r,d))
#define HWIO_VOTETABLE_DRVr_NDd_RESP_REQ_BMSK                                               0x80000000
#define HWIO_VOTETABLE_DRVr_NDd_RESP_REQ_SHFT                                                     0x1f
#define HWIO_VOTETABLE_DRVr_NDd_COMMIT_BMSK                                                 0x40000000
#define HWIO_VOTETABLE_DRVr_NDd_COMMIT_SHFT                                                       0x1e
#define HWIO_VOTETABLE_DRVr_NDd_VOTE_VALID_BMSK                                             0x20000000
#define HWIO_VOTETABLE_DRVr_NDd_VOTE_VALID_SHFT                                                   0x1d
#define HWIO_VOTETABLE_DRVr_NDd_VOTE_X_BMSK                                                  0xfffc000
#define HWIO_VOTETABLE_DRVr_NDd_VOTE_X_SHFT                                                        0xe
#define HWIO_VOTETABLE_DRVr_NDd_VOTE_Y_BMSK                                                     0x3fff
#define HWIO_VOTETABLE_DRVr_NDd_VOTE_Y_SHFT                                                        0x0

#define HWIO_SHADOW_STATUS_DRVr_FE_AGG_FSM_ADDR(r)                                          (BCM_VOTETABLE_REG_BASE      + 0x00000200 + 0x10000 * (r))
#define HWIO_SHADOW_STATUS_DRVr_FE_AGG_FSM_RMSK                                                 0xffff
#define HWIO_SHADOW_STATUS_DRVr_FE_AGG_FSM_MAXr                                                     11
#define HWIO_SHADOW_STATUS_DRVr_FE_AGG_FSM_INI(r)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_FE_AGG_FSM_ADDR(r), HWIO_SHADOW_STATUS_DRVr_FE_AGG_FSM_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_FE_AGG_FSM_INMI(r,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_FE_AGG_FSM_ADDR(r), mask)
#define HWIO_SHADOW_STATUS_DRVr_FE_AGG_FSM_AGG_BUSY_CD_BMSK                                     0xffff
#define HWIO_SHADOW_STATUS_DRVr_FE_AGG_FSM_AGG_BUSY_CD_SHFT                                        0x0

#define HWIO_SHADOW_STATUS_DRVr_FE_CDm_ADDR(r,m)                                            (BCM_VOTETABLE_REG_BASE      + 0x00000204 + 0x10000 * (r) + 0x4 * (m))
#define HWIO_SHADOW_STATUS_DRVr_FE_CDm_RMSK                                                 0x7fff10ff
#define HWIO_SHADOW_STATUS_DRVr_FE_CDm_MAXr                                                         11
#define HWIO_SHADOW_STATUS_DRVr_FE_CDm_MAXm                                                         15
#define HWIO_SHADOW_STATUS_DRVr_FE_CDm_INI2(r,m)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_FE_CDm_ADDR(r,m), HWIO_SHADOW_STATUS_DRVr_FE_CDm_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_FE_CDm_INMI2(r,m,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_FE_CDm_ADDR(r,m), mask)
#define HWIO_SHADOW_STATUS_DRVr_FE_CDm_AGG_BW_BMSK                                          0x7fff0000
#define HWIO_SHADOW_STATUS_DRVr_FE_CDm_AGG_BW_SHFT                                                0x10
#define HWIO_SHADOW_STATUS_DRVr_FE_CDm_COMMIT_CD_RAW_BMSK                                       0x1000
#define HWIO_SHADOW_STATUS_DRVr_FE_CDm_COMMIT_CD_RAW_SHFT                                          0xc
#define HWIO_SHADOW_STATUS_DRVr_FE_CDm_FINAL_CP_BMSK                                              0xf0
#define HWIO_SHADOW_STATUS_DRVr_FE_CDm_FINAL_CP_SHFT                                               0x4
#define HWIO_SHADOW_STATUS_DRVr_FE_CDm_AGG_CP_BMSK                                                 0xf
#define HWIO_SHADOW_STATUS_DRVr_FE_CDm_AGG_CP_SHFT                                                 0x0

#define HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_0_ADDR(r)                                          (BCM_VOTETABLE_REG_BASE      + 0x00000284 + 0x10000 * (r))
#define HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_0_RMSK                                             0xffffffff
#define HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_0_MAXr                                                     11
#define HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_0_INI(r)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_0_ADDR(r), HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_0_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_0_INMI(r,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_0_ADDR(r), mask)
#define HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_0_DIRTY_ND_31_0_BMSK                               0xffffffff
#define HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_0_DIRTY_ND_31_0_SHFT                                      0x0

#define HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_1_ADDR(r)                                          (BCM_VOTETABLE_REG_BASE      + 0x00000288 + 0x10000 * (r))
#define HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_1_RMSK                                             0xffffffff
#define HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_1_MAXr                                                     11
#define HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_1_INI(r)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_1_ADDR(r), HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_1_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_1_INMI(r,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_1_ADDR(r), mask)
#define HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_1_DIRTY_ND_63_32_BMSK                              0xffffffff
#define HWIO_SHADOW_STATUS_DRVr_DIRTY_ND_1_DIRTY_ND_63_32_SHFT                                     0x0

#define HWIO_SHADOW_STATUS_DRVr_SNDs_ADDR(r,s)                                              (BCM_VOTETABLE_REG_BASE      + 0x0000028c + 0x10000 * (r) + 0x4 * (s))
#define HWIO_SHADOW_STATUS_DRVr_SNDs_RMSK                                                          0xf
#define HWIO_SHADOW_STATUS_DRVr_SNDs_MAXr                                                           11
#define HWIO_SHADOW_STATUS_DRVr_SNDs_MAXs                                                            7
#define HWIO_SHADOW_STATUS_DRVr_SNDs_INI2(r,s)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_SNDs_ADDR(r,s), HWIO_SHADOW_STATUS_DRVr_SNDs_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_SNDs_INMI2(r,s,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_SNDs_ADDR(r,s), mask)
#define HWIO_SHADOW_STATUS_DRVr_SNDs_SEL_CP_BMSK                                                   0xf
#define HWIO_SHADOW_STATUS_DRVr_SNDs_SEL_CP_SHFT                                                   0x0

#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_NDd_ADDR(r,d)                                        (BCM_VOTETABLE_REG_BASE      + 0x000002ac + 0x10000 * (r) + 0x4 * (d))
#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_NDd_RMSK                                                 0x7fff
#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_NDd_MAXr                                                     11
#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_NDd_MAXd                                                     63
#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_NDd_INI2(r,d)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_AGG_BW_NDd_ADDR(r,d), HWIO_SHADOW_STATUS_DRVr_AGG_BW_NDd_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_NDd_INMI2(r,d,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_AGG_BW_NDd_ADDR(r,d), mask)
#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_NDd_AGG_BW_BMSK                                          0x7fff
#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_NDd_AGG_BW_SHFT                                             0x0

#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_ADDR(r,s)                                      (BCM_VOTETABLE_REG_BASE      + 0x000004ac + 0x10000 * (r) + 0x4 * (s))
#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_RMSK                                               0x7fff
#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_MAXr                                                   11
#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_MAXs                                                    7
#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_INI2(r,s)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_ADDR(r,s), HWIO_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_INMI2(r,s,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_ADDR(r,s), mask)
#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_AGG_BW_Z_BMSK                                      0x7fff
#define HWIO_SHADOW_STATUS_DRVr_AGG_BW_Z_NDs_AGG_BW_Z_SHFT                                         0x0

#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_ADDR(r,m)                                            (BCM_VOTETABLE_REG_BASE      + 0x000004cc + 0x10000 * (r) + 0x4 * (m))
#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_RMSK                                                    0xfffff
#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_MAXr                                                         11
#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_MAXm                                                         15
#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_INI2(r,m)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_BE_CDm_ADDR(r,m), HWIO_SHADOW_STATUS_DRVr_BE_CDm_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_INMI2(r,m,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_BE_CDm_ADDR(r,m), mask)
#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_CLK_DEST_STATE_BMSK                                     0xf0000
#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_CLK_DEST_STATE_SHFT                                        0x10
#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_COMBINED_CP_BMSK                                         0xf000
#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_COMBINED_CP_SHFT                                            0xc
#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_SW_CP_SNAP_BMSK                                           0xf00
#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_SW_CP_SNAP_SHFT                                             0x8
#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_WRITTEN_CP_BMSK                                            0xf0
#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_WRITTEN_CP_SHFT                                             0x4
#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_CURR_CP_BMSK                                                0xf
#define HWIO_SHADOW_STATUS_DRVr_BE_CDm_CURR_CP_SHFT                                                0x0

#define HWIO_SHADOW_STATUS_DRVr_BE_SEQ_CDm_ADDR(r,m)                                        (BCM_VOTETABLE_REG_BASE      + 0x0000054c + 0x10000 * (r) + 0x4 * (m))
#define HWIO_SHADOW_STATUS_DRVr_BE_SEQ_CDm_RMSK                                                 0xff01
#define HWIO_SHADOW_STATUS_DRVr_BE_SEQ_CDm_MAXr                                                     11
#define HWIO_SHADOW_STATUS_DRVr_BE_SEQ_CDm_MAXm                                                     15
#define HWIO_SHADOW_STATUS_DRVr_BE_SEQ_CDm_INI2(r,m)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_BE_SEQ_CDm_ADDR(r,m), HWIO_SHADOW_STATUS_DRVr_BE_SEQ_CDm_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_BE_SEQ_CDm_INMI2(r,m,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_BE_SEQ_CDm_ADDR(r,m), mask)
#define HWIO_SHADOW_STATUS_DRVr_BE_SEQ_CDm_SEQ_CURR_PC_BMSK                                     0xff00
#define HWIO_SHADOW_STATUS_DRVr_BE_SEQ_CDm_SEQ_CURR_PC_SHFT                                        0x8
#define HWIO_SHADOW_STATUS_DRVr_BE_SEQ_CDm_SEQ_STATE_BMSK                                          0x1
#define HWIO_SHADOW_STATUS_DRVr_BE_SEQ_CDm_SEQ_STATE_SHFT                                          0x0

#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_ADDR(r,m)                           (BCM_VOTETABLE_REG_BASE      + 0x000005cc + 0x10000 * (r) + 0x4 * (m))
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_RMSK                                       0x7
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_MAXr                                        11
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_MAXm                                        15
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_INI2(r,m)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_ADDR(r,m), HWIO_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_INMI2(r,m,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_ADDR(r,m), mask)
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_ISSUE_STATUS_BMSK                          0x7
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_ISSUE_STATUS_CDm_ISSUE_STATUS_SHFT                          0x0

#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_ADDR(r,m)                            (BCM_VOTETABLE_REG_BASE      + 0x0000064c + 0x10000 * (r) + 0x4 * (m))
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_RMSK                                        0x7
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_MAXr                                         11
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_MAXm                                         15
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_INI2(r,m)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_ADDR(r,m), HWIO_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_INMI2(r,m,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_ADDR(r,m), mask)
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_SENT_STATUS_BMSK                            0x7
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_SENT_STATUS_CDm_SENT_STATUS_SHFT                            0x0

#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_ADDR(r,m)                            (BCM_VOTETABLE_REG_BASE      + 0x000006cc + 0x10000 * (r) + 0x4 * (m))
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_RMSK                                        0x7
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_MAXr                                         11
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_MAXm                                         15
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_INI2(r,m)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_ADDR(r,m), HWIO_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_INMI2(r,m,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_ADDR(r,m), mask)
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_CMPL_STATUS_BMSK                            0x7
#define HWIO_SHADOW_STATUS_DRVr_BE_CMD_CMPL_STATUS_CDm_CMPL_STATUS_SHFT                            0x0

#define HWIO_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_ADDR(r,m)                                 (BCM_VOTETABLE_REG_BASE      + 0x0000074c + 0x10000 * (r) + 0x4 * (m))
#define HWIO_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_RMSK                                             0x7
#define HWIO_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_MAXr                                              11
#define HWIO_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_MAXm                                              15
#define HWIO_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_INI2(r,m)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_ADDR(r,m), HWIO_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_INMI2(r,m,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_ADDR(r,m), mask)
#define HWIO_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_TCS_CURR_STATE_BMSK                              0x7
#define HWIO_SHADOW_STATUS_DRVr_BE_TCS_STATUS_CDm_TCS_CURR_STATE_SHFT                              0x0

#define HWIO_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_ADDR(r)                                        (BCM_VOTETABLE_REG_BASE      + 0x0000085c + 0x10000 * (r))
#define HWIO_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_RMSK                                              0xfff01
#define HWIO_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_MAXr                                                   11
#define HWIO_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_INI(r)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_ADDR(r), HWIO_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_INMI(r,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_ADDR(r), mask)
#define HWIO_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_CTT_EMPTY_DRV_BMSK                                0xfff00
#define HWIO_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_CTT_EMPTY_DRV_SHFT                                    0x8
#define HWIO_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_CTT_EMPTY_BMSK                                        0x1
#define HWIO_SHADOW_STATUS_DRVr_CE_CTT_EMPTY_CTT_EMPTY_SHFT                                        0x0

#define HWIO_SHADOW_STATUS_DRVr_BCM_ADDR(r)                                                 (BCM_VOTETABLE_REG_BASE      + 0x00000860 + 0x10000 * (r))
#define HWIO_SHADOW_STATUS_DRVr_BCM_RMSK                                                           0x1
#define HWIO_SHADOW_STATUS_DRVr_BCM_MAXr                                                            11
#define HWIO_SHADOW_STATUS_DRVr_BCM_INI(r)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_BCM_ADDR(r), HWIO_SHADOW_STATUS_DRVr_BCM_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_BCM_INMI(r,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_BCM_ADDR(r), mask)
#define HWIO_SHADOW_STATUS_DRVr_BCM_BCM_IDLE_BMSK                                                  0x1
#define HWIO_SHADOW_STATUS_DRVr_BCM_BCM_IDLE_SHFT                                                  0x0

#define HWIO_SHADOW_STATUS_DRVr_HW_VERSION_ADDR(r)                                          (BCM_VOTETABLE_REG_BASE      + 0x00000864 + 0x10000 * (r))
#define HWIO_SHADOW_STATUS_DRVr_HW_VERSION_RMSK                                               0xffffff
#define HWIO_SHADOW_STATUS_DRVr_HW_VERSION_MAXr                                                     11
#define HWIO_SHADOW_STATUS_DRVr_HW_VERSION_INI(r)        \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_HW_VERSION_ADDR(r), HWIO_SHADOW_STATUS_DRVr_HW_VERSION_RMSK)
#define HWIO_SHADOW_STATUS_DRVr_HW_VERSION_INMI(r,mask)    \
        in_dword_masked(HWIO_SHADOW_STATUS_DRVr_HW_VERSION_ADDR(r), mask)
#define HWIO_SHADOW_STATUS_DRVr_HW_VERSION_MAJOR_BMSK                                         0xff0000
#define HWIO_SHADOW_STATUS_DRVr_HW_VERSION_MAJOR_SHFT                                             0x10
#define HWIO_SHADOW_STATUS_DRVr_HW_VERSION_MINOR_BMSK                                           0xff00
#define HWIO_SHADOW_STATUS_DRVr_HW_VERSION_MINOR_SHFT                                              0x8
#define HWIO_SHADOW_STATUS_DRVr_HW_VERSION_STEP_BMSK                                              0xff
#define HWIO_SHADOW_STATUS_DRVr_HW_VERSION_STEP_SHFT                                               0x0

/*----------------------------------------------------------------------------
 * MODULE: BCM_ND_CFG
 *--------------------------------------------------------------------------*/

#define BCM_ND_CFG_REG_BASE                                             (AOSS_BASE      + 0x00be0000)

#define HWIO_ND_CFG_NDd_ADDR(d)                                         (BCM_ND_CFG_REG_BASE      + 0x00000000 + 0x4 * (d))
#define HWIO_ND_CFG_NDd_RMSK                                                 0x1ff
#define HWIO_ND_CFG_NDd_MAXd                                                    63
#define HWIO_ND_CFG_NDd_INI(d)        \
        in_dword_masked(HWIO_ND_CFG_NDd_ADDR(d), HWIO_ND_CFG_NDd_RMSK)
#define HWIO_ND_CFG_NDd_INMI(d,mask)    \
        in_dword_masked(HWIO_ND_CFG_NDd_ADDR(d), mask)
#define HWIO_ND_CFG_NDd_OUTI(d,val)    \
        out_dword(HWIO_ND_CFG_NDd_ADDR(d),val)
#define HWIO_ND_CFG_NDd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_ND_CFG_NDd_ADDR(d),mask,val,HWIO_ND_CFG_NDd_INI(d))
#define HWIO_ND_CFG_NDd_ND_ALGO_SEL_BMSK                                     0x1f0
#define HWIO_ND_CFG_NDd_ND_ALGO_SEL_SHFT                                       0x4
#define HWIO_ND_CFG_NDd_CLOCK_DOMAIN_ID_BMSK                                   0xf
#define HWIO_ND_CFG_NDd_CLOCK_DOMAIN_ID_SHFT                                   0x0

#define HWIO_SND_CFG_SNDs_ADDR(s)                                       (BCM_ND_CFG_REG_BASE      + 0x00000200 + 0x4 * (s))
#define HWIO_SND_CFG_SNDs_RMSK                                                0x3f
#define HWIO_SND_CFG_SNDs_MAXs                                                   7
#define HWIO_SND_CFG_SNDs_INI(s)        \
        in_dword_masked(HWIO_SND_CFG_SNDs_ADDR(s), HWIO_SND_CFG_SNDs_RMSK)
#define HWIO_SND_CFG_SNDs_INMI(s,mask)    \
        in_dword_masked(HWIO_SND_CFG_SNDs_ADDR(s), mask)
#define HWIO_SND_CFG_SNDs_OUTI(s,val)    \
        out_dword(HWIO_SND_CFG_SNDs_ADDR(s),val)
#define HWIO_SND_CFG_SNDs_OUTMI(s,mask,val) \
        out_dword_masked_ns(HWIO_SND_CFG_SNDs_ADDR(s),mask,val,HWIO_SND_CFG_SNDs_INI(s))
#define HWIO_SND_CFG_SNDs_Z_ND_ASSOCIATION_BMSK                               0x3f
#define HWIO_SND_CFG_SNDs_Z_ND_ASSOCIATION_SHFT                                0x0

#define HWIO_SND_LUT_SNDs_COLc_ADDR(s,c)                                (BCM_ND_CFG_REG_BASE      + 0x00000300 + 0x10 * (s) + 0x4 * (c))
#define HWIO_SND_LUT_SNDs_COLc_RMSK                                         0x3fff
#define HWIO_SND_LUT_SNDs_COLc_MAXs                                              7
#define HWIO_SND_LUT_SNDs_COLc_MAXc                                              3
#define HWIO_SND_LUT_SNDs_COLc_INI2(s,c)        \
        in_dword_masked(HWIO_SND_LUT_SNDs_COLc_ADDR(s,c), HWIO_SND_LUT_SNDs_COLc_RMSK)
#define HWIO_SND_LUT_SNDs_COLc_INMI2(s,c,mask)    \
        in_dword_masked(HWIO_SND_LUT_SNDs_COLc_ADDR(s,c), mask)
#define HWIO_SND_LUT_SNDs_COLc_OUTI2(s,c,val)    \
        out_dword(HWIO_SND_LUT_SNDs_COLc_ADDR(s,c),val)
#define HWIO_SND_LUT_SNDs_COLc_OUTMI2(s,c,mask,val) \
        out_dword_masked_ns(HWIO_SND_LUT_SNDs_COLc_ADDR(s,c),mask,val,HWIO_SND_LUT_SNDs_COLc_INI2(s,c))
#define HWIO_SND_LUT_SNDs_COLc_COL_THRESHOLD_BMSK                           0x3fff
#define HWIO_SND_LUT_SNDs_COLc_COL_THRESHOLD_SHFT                              0x0

#define HWIO_SND_LUT_SND0_COLc_CPn_ADDR(c,n)                            (BCM_ND_CFG_REG_BASE      + 0x00000500 + 0x40 * (c) + 0x4 * (n))
#define HWIO_SND_LUT_SND0_COLc_CPn_RMSK                                     0x3fff
#define HWIO_SND_LUT_SND0_COLc_CPn_MAXc                                          3
#define HWIO_SND_LUT_SND0_COLc_CPn_MAXn                                         15
#define HWIO_SND_LUT_SND0_COLc_CPn_INI2(c,n)        \
        in_dword_masked(HWIO_SND_LUT_SND0_COLc_CPn_ADDR(c,n), HWIO_SND_LUT_SND0_COLc_CPn_RMSK)
#define HWIO_SND_LUT_SND0_COLc_CPn_INMI2(c,n,mask)    \
        in_dword_masked(HWIO_SND_LUT_SND0_COLc_CPn_ADDR(c,n), mask)
#define HWIO_SND_LUT_SND0_COLc_CPn_OUTI2(c,n,val)    \
        out_dword(HWIO_SND_LUT_SND0_COLc_CPn_ADDR(c,n),val)
#define HWIO_SND_LUT_SND0_COLc_CPn_OUTMI2(c,n,mask,val) \
        out_dword_masked_ns(HWIO_SND_LUT_SND0_COLc_CPn_ADDR(c,n),mask,val,HWIO_SND_LUT_SND0_COLc_CPn_INI2(c,n))
#define HWIO_SND_LUT_SND0_COLc_CPn_ROW_THRESHOLD_BMSK                       0x3fff
#define HWIO_SND_LUT_SND0_COLc_CPn_ROW_THRESHOLD_SHFT                          0x0

#define HWIO_SND_LUT_SND1_COLc_CPn_ADDR(c,n)                            (BCM_ND_CFG_REG_BASE      + 0x00000600 + 0x40 * (c) + 0x4 * (n))
#define HWIO_SND_LUT_SND1_COLc_CPn_RMSK                                     0x3fff
#define HWIO_SND_LUT_SND1_COLc_CPn_MAXc                                          3
#define HWIO_SND_LUT_SND1_COLc_CPn_MAXn                                         15
#define HWIO_SND_LUT_SND1_COLc_CPn_INI2(c,n)        \
        in_dword_masked(HWIO_SND_LUT_SND1_COLc_CPn_ADDR(c,n), HWIO_SND_LUT_SND1_COLc_CPn_RMSK)
#define HWIO_SND_LUT_SND1_COLc_CPn_INMI2(c,n,mask)    \
        in_dword_masked(HWIO_SND_LUT_SND1_COLc_CPn_ADDR(c,n), mask)
#define HWIO_SND_LUT_SND1_COLc_CPn_OUTI2(c,n,val)    \
        out_dword(HWIO_SND_LUT_SND1_COLc_CPn_ADDR(c,n),val)
#define HWIO_SND_LUT_SND1_COLc_CPn_OUTMI2(c,n,mask,val) \
        out_dword_masked_ns(HWIO_SND_LUT_SND1_COLc_CPn_ADDR(c,n),mask,val,HWIO_SND_LUT_SND1_COLc_CPn_INI2(c,n))
#define HWIO_SND_LUT_SND1_COLc_CPn_ROW_THRESHOLD_BMSK                       0x3fff
#define HWIO_SND_LUT_SND1_COLc_CPn_ROW_THRESHOLD_SHFT                          0x0

#define HWIO_SND_LUT_SND2_COLc_CPn_ADDR(c,n)                            (BCM_ND_CFG_REG_BASE      + 0x00000700 + 0x40 * (c) + 0x4 * (n))
#define HWIO_SND_LUT_SND2_COLc_CPn_RMSK                                     0x3fff
#define HWIO_SND_LUT_SND2_COLc_CPn_MAXc                                          3
#define HWIO_SND_LUT_SND2_COLc_CPn_MAXn                                         15
#define HWIO_SND_LUT_SND2_COLc_CPn_INI2(c,n)        \
        in_dword_masked(HWIO_SND_LUT_SND2_COLc_CPn_ADDR(c,n), HWIO_SND_LUT_SND2_COLc_CPn_RMSK)
#define HWIO_SND_LUT_SND2_COLc_CPn_INMI2(c,n,mask)    \
        in_dword_masked(HWIO_SND_LUT_SND2_COLc_CPn_ADDR(c,n), mask)
#define HWIO_SND_LUT_SND2_COLc_CPn_OUTI2(c,n,val)    \
        out_dword(HWIO_SND_LUT_SND2_COLc_CPn_ADDR(c,n),val)
#define HWIO_SND_LUT_SND2_COLc_CPn_OUTMI2(c,n,mask,val) \
        out_dword_masked_ns(HWIO_SND_LUT_SND2_COLc_CPn_ADDR(c,n),mask,val,HWIO_SND_LUT_SND2_COLc_CPn_INI2(c,n))
#define HWIO_SND_LUT_SND2_COLc_CPn_ROW_THRESHOLD_BMSK                       0x3fff
#define HWIO_SND_LUT_SND2_COLc_CPn_ROW_THRESHOLD_SHFT                          0x0

#define HWIO_SND_LUT_SND3_COLc_CPn_ADDR(c,n)                            (BCM_ND_CFG_REG_BASE      + 0x00000800 + 0x40 * (c) + 0x4 * (n))
#define HWIO_SND_LUT_SND3_COLc_CPn_RMSK                                     0x3fff
#define HWIO_SND_LUT_SND3_COLc_CPn_MAXc                                          3
#define HWIO_SND_LUT_SND3_COLc_CPn_MAXn                                         15
#define HWIO_SND_LUT_SND3_COLc_CPn_INI2(c,n)        \
        in_dword_masked(HWIO_SND_LUT_SND3_COLc_CPn_ADDR(c,n), HWIO_SND_LUT_SND3_COLc_CPn_RMSK)
#define HWIO_SND_LUT_SND3_COLc_CPn_INMI2(c,n,mask)    \
        in_dword_masked(HWIO_SND_LUT_SND3_COLc_CPn_ADDR(c,n), mask)
#define HWIO_SND_LUT_SND3_COLc_CPn_OUTI2(c,n,val)    \
        out_dword(HWIO_SND_LUT_SND3_COLc_CPn_ADDR(c,n),val)
#define HWIO_SND_LUT_SND3_COLc_CPn_OUTMI2(c,n,mask,val) \
        out_dword_masked_ns(HWIO_SND_LUT_SND3_COLc_CPn_ADDR(c,n),mask,val,HWIO_SND_LUT_SND3_COLc_CPn_INI2(c,n))
#define HWIO_SND_LUT_SND3_COLc_CPn_ROW_THRESHOLD_BMSK                       0x3fff
#define HWIO_SND_LUT_SND3_COLc_CPn_ROW_THRESHOLD_SHFT                          0x0

#define HWIO_SND_LUT_SND4_COLc_CPn_ADDR(c,n)                            (BCM_ND_CFG_REG_BASE      + 0x00000900 + 0x40 * (c) + 0x4 * (n))
#define HWIO_SND_LUT_SND4_COLc_CPn_RMSK                                     0x3fff
#define HWIO_SND_LUT_SND4_COLc_CPn_MAXc                                          3
#define HWIO_SND_LUT_SND4_COLc_CPn_MAXn                                          7
#define HWIO_SND_LUT_SND4_COLc_CPn_INI2(c,n)        \
        in_dword_masked(HWIO_SND_LUT_SND4_COLc_CPn_ADDR(c,n), HWIO_SND_LUT_SND4_COLc_CPn_RMSK)
#define HWIO_SND_LUT_SND4_COLc_CPn_INMI2(c,n,mask)    \
        in_dword_masked(HWIO_SND_LUT_SND4_COLc_CPn_ADDR(c,n), mask)
#define HWIO_SND_LUT_SND4_COLc_CPn_OUTI2(c,n,val)    \
        out_dword(HWIO_SND_LUT_SND4_COLc_CPn_ADDR(c,n),val)
#define HWIO_SND_LUT_SND4_COLc_CPn_OUTMI2(c,n,mask,val) \
        out_dword_masked_ns(HWIO_SND_LUT_SND4_COLc_CPn_ADDR(c,n),mask,val,HWIO_SND_LUT_SND4_COLc_CPn_INI2(c,n))
#define HWIO_SND_LUT_SND4_COLc_CPn_ROW_THRESHOLD_BMSK                       0x3fff
#define HWIO_SND_LUT_SND4_COLc_CPn_ROW_THRESHOLD_SHFT                          0x0

#define HWIO_SND_LUT_SND5_COLc_CPn_ADDR(c,n)                            (BCM_ND_CFG_REG_BASE      + 0x00000a00 + 0x40 * (c) + 0x4 * (n))
#define HWIO_SND_LUT_SND5_COLc_CPn_RMSK                                     0x3fff
#define HWIO_SND_LUT_SND5_COLc_CPn_MAXc                                          3
#define HWIO_SND_LUT_SND5_COLc_CPn_MAXn                                          7
#define HWIO_SND_LUT_SND5_COLc_CPn_INI2(c,n)        \
        in_dword_masked(HWIO_SND_LUT_SND5_COLc_CPn_ADDR(c,n), HWIO_SND_LUT_SND5_COLc_CPn_RMSK)
#define HWIO_SND_LUT_SND5_COLc_CPn_INMI2(c,n,mask)    \
        in_dword_masked(HWIO_SND_LUT_SND5_COLc_CPn_ADDR(c,n), mask)
#define HWIO_SND_LUT_SND5_COLc_CPn_OUTI2(c,n,val)    \
        out_dword(HWIO_SND_LUT_SND5_COLc_CPn_ADDR(c,n),val)
#define HWIO_SND_LUT_SND5_COLc_CPn_OUTMI2(c,n,mask,val) \
        out_dword_masked_ns(HWIO_SND_LUT_SND5_COLc_CPn_ADDR(c,n),mask,val,HWIO_SND_LUT_SND5_COLc_CPn_INI2(c,n))
#define HWIO_SND_LUT_SND5_COLc_CPn_ROW_THRESHOLD_BMSK                       0x3fff
#define HWIO_SND_LUT_SND5_COLc_CPn_ROW_THRESHOLD_SHFT                          0x0

#define HWIO_SND_LUT_SND6_COLc_CPn_ADDR(c,n)                            (BCM_ND_CFG_REG_BASE      + 0x00000b00 + 0x40 * (c) + 0x4 * (n))
#define HWIO_SND_LUT_SND6_COLc_CPn_RMSK                                     0x3fff
#define HWIO_SND_LUT_SND6_COLc_CPn_MAXc                                          3
#define HWIO_SND_LUT_SND6_COLc_CPn_MAXn                                          7
#define HWIO_SND_LUT_SND6_COLc_CPn_INI2(c,n)        \
        in_dword_masked(HWIO_SND_LUT_SND6_COLc_CPn_ADDR(c,n), HWIO_SND_LUT_SND6_COLc_CPn_RMSK)
#define HWIO_SND_LUT_SND6_COLc_CPn_INMI2(c,n,mask)    \
        in_dword_masked(HWIO_SND_LUT_SND6_COLc_CPn_ADDR(c,n), mask)
#define HWIO_SND_LUT_SND6_COLc_CPn_OUTI2(c,n,val)    \
        out_dword(HWIO_SND_LUT_SND6_COLc_CPn_ADDR(c,n),val)
#define HWIO_SND_LUT_SND6_COLc_CPn_OUTMI2(c,n,mask,val) \
        out_dword_masked_ns(HWIO_SND_LUT_SND6_COLc_CPn_ADDR(c,n),mask,val,HWIO_SND_LUT_SND6_COLc_CPn_INI2(c,n))
#define HWIO_SND_LUT_SND6_COLc_CPn_ROW_THRESHOLD_BMSK                       0x3fff
#define HWIO_SND_LUT_SND6_COLc_CPn_ROW_THRESHOLD_SHFT                          0x0

#define HWIO_SND_LUT_SND7_COLc_CPn_ADDR(c,n)                            (BCM_ND_CFG_REG_BASE      + 0x00000c00 + 0x40 * (c) + 0x4 * (n))
#define HWIO_SND_LUT_SND7_COLc_CPn_RMSK                                     0x3fff
#define HWIO_SND_LUT_SND7_COLc_CPn_MAXc                                          3
#define HWIO_SND_LUT_SND7_COLc_CPn_MAXn                                          7
#define HWIO_SND_LUT_SND7_COLc_CPn_INI2(c,n)        \
        in_dword_masked(HWIO_SND_LUT_SND7_COLc_CPn_ADDR(c,n), HWIO_SND_LUT_SND7_COLc_CPn_RMSK)
#define HWIO_SND_LUT_SND7_COLc_CPn_INMI2(c,n,mask)    \
        in_dword_masked(HWIO_SND_LUT_SND7_COLc_CPn_ADDR(c,n), mask)
#define HWIO_SND_LUT_SND7_COLc_CPn_OUTI2(c,n,val)    \
        out_dword(HWIO_SND_LUT_SND7_COLc_CPn_ADDR(c,n),val)
#define HWIO_SND_LUT_SND7_COLc_CPn_OUTMI2(c,n,mask,val) \
        out_dword_masked_ns(HWIO_SND_LUT_SND7_COLc_CPn_ADDR(c,n),mask,val,HWIO_SND_LUT_SND7_COLc_CPn_INI2(c,n))
#define HWIO_SND_LUT_SND7_COLc_CPn_ROW_THRESHOLD_BMSK                       0x3fff
#define HWIO_SND_LUT_SND7_COLc_CPn_ROW_THRESHOLD_SHFT                          0x0

/*----------------------------------------------------------------------------
 * MODULE: BCM_CD_CFG
 *--------------------------------------------------------------------------*/

#define BCM_CD_CFG_REG_BASE                                     (AOSS_BASE      + 0x00be3000)

#define HWIO_CD_CFG_CDm_ADDR(m)                                 (BCM_CD_CFG_REG_BASE      + 0x00000000 + 0x4 * (m))
#define HWIO_CD_CFG_CDm_RMSK                                       0xf0f1f
#define HWIO_CD_CFG_CDm_MAXm                                            15
#define HWIO_CD_CFG_CDm_INI(m)        \
        in_dword_masked(HWIO_CD_CFG_CDm_ADDR(m), HWIO_CD_CFG_CDm_RMSK)
#define HWIO_CD_CFG_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_CD_CFG_CDm_ADDR(m), mask)
#define HWIO_CD_CFG_CDm_OUTI(m,val)    \
        out_dword(HWIO_CD_CFG_CDm_ADDR(m),val)
#define HWIO_CD_CFG_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_CD_CFG_CDm_ADDR(m),mask,val,HWIO_CD_CFG_CDm_INI(m))
#define HWIO_CD_CFG_CDm_PC_TH_BMSK                                 0xf0000
#define HWIO_CD_CFG_CDm_PC_TH_SHFT                                    0x10
#define HWIO_CD_CFG_CDm_DEPENDENT_CD_ID_BMSK                         0xf00
#define HWIO_CD_CFG_CDm_DEPENDENT_CD_ID_SHFT                           0x8
#define HWIO_CD_CFG_CDm_CD_ALGO_SEL_BMSK                              0x1f
#define HWIO_CD_CFG_CDm_CD_ALGO_SEL_SHFT                               0x0

#define HWIO_CD_CFG_ENABLE_CDm_ADDR(m)                          (BCM_CD_CFG_REG_BASE      + 0x00000080 + 0x4 * (m))
#define HWIO_CD_CFG_ENABLE_CDm_RMSK                                    0x1
#define HWIO_CD_CFG_ENABLE_CDm_MAXm                                     15
#define HWIO_CD_CFG_ENABLE_CDm_INI(m)        \
        in_dword_masked(HWIO_CD_CFG_ENABLE_CDm_ADDR(m), HWIO_CD_CFG_ENABLE_CDm_RMSK)
#define HWIO_CD_CFG_ENABLE_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_CD_CFG_ENABLE_CDm_ADDR(m), mask)
#define HWIO_CD_CFG_ENABLE_CDm_OUTI(m,val)    \
        out_dword(HWIO_CD_CFG_ENABLE_CDm_ADDR(m),val)
#define HWIO_CD_CFG_ENABLE_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_CD_CFG_ENABLE_CDm_ADDR(m),mask,val,HWIO_CD_CFG_ENABLE_CDm_INI(m))
#define HWIO_CD_CFG_ENABLE_CDm_SW_ENABLE_BMSK                          0x1
#define HWIO_CD_CFG_ENABLE_CDm_SW_ENABLE_SHFT                          0x0

#define HWIO_CD_CFG_CDm_AU_INPUTz_ADDR(m,z)                     (BCM_CD_CFG_REG_BASE      + 0x00000200 + 0x40 * (m) + 0x4 * (z))
#define HWIO_CD_CFG_CDm_AU_INPUTz_RMSK                                0x3f
#define HWIO_CD_CFG_CDm_AU_INPUTz_MAXm                                  15
#define HWIO_CD_CFG_CDm_AU_INPUTz_MAXz                                  15
#define HWIO_CD_CFG_CDm_AU_INPUTz_INI2(m,z)        \
        in_dword_masked(HWIO_CD_CFG_CDm_AU_INPUTz_ADDR(m,z), HWIO_CD_CFG_CDm_AU_INPUTz_RMSK)
#define HWIO_CD_CFG_CDm_AU_INPUTz_INMI2(m,z,mask)    \
        in_dword_masked(HWIO_CD_CFG_CDm_AU_INPUTz_ADDR(m,z), mask)
#define HWIO_CD_CFG_CDm_AU_INPUTz_OUTI2(m,z,val)    \
        out_dword(HWIO_CD_CFG_CDm_AU_INPUTz_ADDR(m,z),val)
#define HWIO_CD_CFG_CDm_AU_INPUTz_OUTMI2(m,z,mask,val) \
        out_dword_masked_ns(HWIO_CD_CFG_CDm_AU_INPUTz_ADDR(m,z),mask,val,HWIO_CD_CFG_CDm_AU_INPUTz_INI2(m,z))
#define HWIO_CD_CFG_CDm_AU_INPUTz_ND_ID_BMSK                          0x3f
#define HWIO_CD_CFG_CDm_AU_INPUTz_ND_ID_SHFT                           0x0

#define HWIO_CD_TH_CD0_CPn_ADDR(n)                              (BCM_CD_CFG_REG_BASE      + 0x00000800 + 0x4 * (n))
#define HWIO_CD_TH_CD0_CPn_RMSK                                     0x3fff
#define HWIO_CD_TH_CD0_CPn_MAXn                                         15
#define HWIO_CD_TH_CD0_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD0_CPn_ADDR(n), HWIO_CD_TH_CD0_CPn_RMSK)
#define HWIO_CD_TH_CD0_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD0_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD0_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD0_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD0_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD0_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD0_CPn_INI(n))
#define HWIO_CD_TH_CD0_CPn_BW_BMSK                                  0x3fff
#define HWIO_CD_TH_CD0_CPn_BW_SHFT                                     0x0

#define HWIO_CD_TH_CDm_CPn_ADDR(m,n)                            (BCM_CD_CFG_REG_BASE      + 0x00000800 + 0x4 * (n) + 0x40 * (m))
#define HWIO_CD_TH_CDm_CPn_RMSK                                     0x3fff
#define HWIO_CD_TH_CDm_CPn_MAXm                                         15
#define HWIO_CD_TH_CDm_CPn_MAXn                                         15
#define HWIO_CD_TH_CDm_CPn_INI2(m,n)        \
        in_dword_masked(HWIO_CD_TH_CDm_CPn_ADDR(m,n), HWIO_CD_TH_CDm_CPn_RMSK)
#define HWIO_CD_TH_CDm_CPn_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_CD_TH_CDm_CPn_ADDR(m,n), mask)
#define HWIO_CD_TH_CDm_CPn_OUTI2(m,n,val)    \
        out_dword(HWIO_CD_TH_CDm_CPn_ADDR(m,n),val)
#define HWIO_CD_TH_CDm_CPn_OUTMI2(m,n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CDm_CPn_ADDR(m,n),mask,val,HWIO_CD_TH_CDm_CPn_INI2(m,n))
#define HWIO_CD_TH_CDm_CPn_BW_BMSK                                  0x3fff
#define HWIO_CD_TH_CDm_CPn_BW_SHFT                                     0x0

#define HWIO_CD_TH_CD1_CPn_ADDR(n)                              (BCM_CD_CFG_REG_BASE      + 0x00000840 + 0x4 * (n))
#define HWIO_CD_TH_CD1_CPn_RMSK                                     0x3fff
#define HWIO_CD_TH_CD1_CPn_MAXn                                         15
#define HWIO_CD_TH_CD1_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD1_CPn_ADDR(n), HWIO_CD_TH_CD1_CPn_RMSK)
#define HWIO_CD_TH_CD1_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD1_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD1_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD1_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD1_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD1_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD1_CPn_INI(n))
#define HWIO_CD_TH_CD1_CPn_BW_BMSK                                  0x3fff
#define HWIO_CD_TH_CD1_CPn_BW_SHFT                                     0x0

#define HWIO_CD_TH_CD2_CPn_ADDR(n)                              (BCM_CD_CFG_REG_BASE      + 0x00000880 + 0x4 * (n))
#define HWIO_CD_TH_CD2_CPn_RMSK                                     0x3fff
#define HWIO_CD_TH_CD2_CPn_MAXn                                          7
#define HWIO_CD_TH_CD2_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD2_CPn_ADDR(n), HWIO_CD_TH_CD2_CPn_RMSK)
#define HWIO_CD_TH_CD2_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD2_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD2_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD2_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD2_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD2_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD2_CPn_INI(n))
#define HWIO_CD_TH_CD2_CPn_BW_BMSK                                  0x3fff
#define HWIO_CD_TH_CD2_CPn_BW_SHFT                                     0x0

#define HWIO_CD_TH_CD3_CPn_ADDR(n)                              (BCM_CD_CFG_REG_BASE      + 0x000008c0 + 0x4 * (n))
#define HWIO_CD_TH_CD3_CPn_RMSK                                     0x3fff
#define HWIO_CD_TH_CD3_CPn_MAXn                                          7
#define HWIO_CD_TH_CD3_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD3_CPn_ADDR(n), HWIO_CD_TH_CD3_CPn_RMSK)
#define HWIO_CD_TH_CD3_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD3_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD3_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD3_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD3_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD3_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD3_CPn_INI(n))
#define HWIO_CD_TH_CD3_CPn_BW_BMSK                                  0x3fff
#define HWIO_CD_TH_CD3_CPn_BW_SHFT                                     0x0

#define HWIO_CD_TH_CD4_CPn_ADDR(n)                              (BCM_CD_CFG_REG_BASE      + 0x00000900 + 0x4 * (n))
#define HWIO_CD_TH_CD4_CPn_RMSK                                     0x3fff
#define HWIO_CD_TH_CD4_CPn_MAXn                                          7
#define HWIO_CD_TH_CD4_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD4_CPn_ADDR(n), HWIO_CD_TH_CD4_CPn_RMSK)
#define HWIO_CD_TH_CD4_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD4_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD4_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD4_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD4_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD4_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD4_CPn_INI(n))
#define HWIO_CD_TH_CD4_CPn_BW_BMSK                                  0x3fff
#define HWIO_CD_TH_CD4_CPn_BW_SHFT                                     0x0

#define HWIO_CD_TH_CD5_CPn_ADDR(n)                              (BCM_CD_CFG_REG_BASE      + 0x00000940 + 0x4 * (n))
#define HWIO_CD_TH_CD5_CPn_RMSK                                     0x3fff
#define HWIO_CD_TH_CD5_CPn_MAXn                                          7
#define HWIO_CD_TH_CD5_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD5_CPn_ADDR(n), HWIO_CD_TH_CD5_CPn_RMSK)
#define HWIO_CD_TH_CD5_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD5_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD5_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD5_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD5_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD5_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD5_CPn_INI(n))
#define HWIO_CD_TH_CD5_CPn_BW_BMSK                                  0x3fff
#define HWIO_CD_TH_CD5_CPn_BW_SHFT                                     0x0

#define HWIO_CD_TH_CD6_CPn_ADDR(n)                              (BCM_CD_CFG_REG_BASE      + 0x00000980 + 0x4 * (n))
#define HWIO_CD_TH_CD6_CPn_RMSK                                     0x3fff
#define HWIO_CD_TH_CD6_CPn_MAXn                                          7
#define HWIO_CD_TH_CD6_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD6_CPn_ADDR(n), HWIO_CD_TH_CD6_CPn_RMSK)
#define HWIO_CD_TH_CD6_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD6_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD6_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD6_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD6_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD6_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD6_CPn_INI(n))
#define HWIO_CD_TH_CD6_CPn_BW_BMSK                                  0x3fff
#define HWIO_CD_TH_CD6_CPn_BW_SHFT                                     0x0

#define HWIO_CD_TH_CD7_CPn_ADDR(n)                              (BCM_CD_CFG_REG_BASE      + 0x000009c0 + 0x4 * (n))
#define HWIO_CD_TH_CD7_CPn_RMSK                                     0x3fff
#define HWIO_CD_TH_CD7_CPn_MAXn                                          7
#define HWIO_CD_TH_CD7_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD7_CPn_ADDR(n), HWIO_CD_TH_CD7_CPn_RMSK)
#define HWIO_CD_TH_CD7_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD7_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD7_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD7_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD7_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD7_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD7_CPn_INI(n))
#define HWIO_CD_TH_CD7_CPn_BW_BMSK                                  0x3fff
#define HWIO_CD_TH_CD7_CPn_BW_SHFT                                     0x0

#define HWIO_CD_TH_CD8_CPn_ADDR(n)                              (BCM_CD_CFG_REG_BASE      + 0x00000a00 + 0x4 * (n))
#define HWIO_CD_TH_CD8_CPn_RMSK                                     0x3fff
#define HWIO_CD_TH_CD8_CPn_MAXn                                          7
#define HWIO_CD_TH_CD8_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD8_CPn_ADDR(n), HWIO_CD_TH_CD8_CPn_RMSK)
#define HWIO_CD_TH_CD8_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD8_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD8_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD8_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD8_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD8_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD8_CPn_INI(n))
#define HWIO_CD_TH_CD8_CPn_BW_BMSK                                  0x3fff
#define HWIO_CD_TH_CD8_CPn_BW_SHFT                                     0x0

#define HWIO_CD_TH_CD9_CPn_ADDR(n)                              (BCM_CD_CFG_REG_BASE      + 0x00000a40 + 0x4 * (n))
#define HWIO_CD_TH_CD9_CPn_RMSK                                     0x3fff
#define HWIO_CD_TH_CD9_CPn_MAXn                                          7
#define HWIO_CD_TH_CD9_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD9_CPn_ADDR(n), HWIO_CD_TH_CD9_CPn_RMSK)
#define HWIO_CD_TH_CD9_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD9_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD9_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD9_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD9_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD9_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD9_CPn_INI(n))
#define HWIO_CD_TH_CD9_CPn_BW_BMSK                                  0x3fff
#define HWIO_CD_TH_CD9_CPn_BW_SHFT                                     0x0

#define HWIO_CD_TH_CD10_CPn_ADDR(n)                             (BCM_CD_CFG_REG_BASE      + 0x00000a80 + 0x4 * (n))
#define HWIO_CD_TH_CD10_CPn_RMSK                                    0x3fff
#define HWIO_CD_TH_CD10_CPn_MAXn                                        15
#define HWIO_CD_TH_CD10_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD10_CPn_ADDR(n), HWIO_CD_TH_CD10_CPn_RMSK)
#define HWIO_CD_TH_CD10_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD10_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD10_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD10_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD10_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD10_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD10_CPn_INI(n))
#define HWIO_CD_TH_CD10_CPn_BW_BMSK                                 0x3fff
#define HWIO_CD_TH_CD10_CPn_BW_SHFT                                    0x0

#define HWIO_CD_TH_CD11_CPn_ADDR(n)                             (BCM_CD_CFG_REG_BASE      + 0x00000ac0 + 0x4 * (n))
#define HWIO_CD_TH_CD11_CPn_RMSK                                    0x3fff
#define HWIO_CD_TH_CD11_CPn_MAXn                                        15
#define HWIO_CD_TH_CD11_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD11_CPn_ADDR(n), HWIO_CD_TH_CD11_CPn_RMSK)
#define HWIO_CD_TH_CD11_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD11_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD11_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD11_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD11_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD11_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD11_CPn_INI(n))
#define HWIO_CD_TH_CD11_CPn_BW_BMSK                                 0x3fff
#define HWIO_CD_TH_CD11_CPn_BW_SHFT                                    0x0

#define HWIO_CD_TH_CD12_CPn_ADDR(n)                             (BCM_CD_CFG_REG_BASE      + 0x00000b00 + 0x4 * (n))
#define HWIO_CD_TH_CD12_CPn_RMSK                                    0x3fff
#define HWIO_CD_TH_CD12_CPn_MAXn                                         7
#define HWIO_CD_TH_CD12_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD12_CPn_ADDR(n), HWIO_CD_TH_CD12_CPn_RMSK)
#define HWIO_CD_TH_CD12_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD12_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD12_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD12_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD12_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD12_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD12_CPn_INI(n))
#define HWIO_CD_TH_CD12_CPn_BW_BMSK                                 0x3fff
#define HWIO_CD_TH_CD12_CPn_BW_SHFT                                    0x0

#define HWIO_CD_TH_CD13_CPn_ADDR(n)                             (BCM_CD_CFG_REG_BASE      + 0x00000b40 + 0x4 * (n))
#define HWIO_CD_TH_CD13_CPn_RMSK                                    0x3fff
#define HWIO_CD_TH_CD13_CPn_MAXn                                         7
#define HWIO_CD_TH_CD13_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD13_CPn_ADDR(n), HWIO_CD_TH_CD13_CPn_RMSK)
#define HWIO_CD_TH_CD13_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD13_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD13_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD13_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD13_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD13_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD13_CPn_INI(n))
#define HWIO_CD_TH_CD13_CPn_BW_BMSK                                 0x3fff
#define HWIO_CD_TH_CD13_CPn_BW_SHFT                                    0x0

#define HWIO_CD_TH_CD14_CPn_ADDR(n)                             (BCM_CD_CFG_REG_BASE      + 0x00000b80 + 0x4 * (n))
#define HWIO_CD_TH_CD14_CPn_RMSK                                    0x3fff
#define HWIO_CD_TH_CD14_CPn_MAXn                                        15
#define HWIO_CD_TH_CD14_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD14_CPn_ADDR(n), HWIO_CD_TH_CD14_CPn_RMSK)
#define HWIO_CD_TH_CD14_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD14_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD14_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD14_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD14_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD14_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD14_CPn_INI(n))
#define HWIO_CD_TH_CD14_CPn_BW_BMSK                                 0x3fff
#define HWIO_CD_TH_CD14_CPn_BW_SHFT                                    0x0

#define HWIO_CD_TH_CD15_CPn_ADDR(n)                             (BCM_CD_CFG_REG_BASE      + 0x00000bc0 + 0x4 * (n))
#define HWIO_CD_TH_CD15_CPn_RMSK                                    0x3fff
#define HWIO_CD_TH_CD15_CPn_MAXn                                        15
#define HWIO_CD_TH_CD15_CPn_INI(n)        \
        in_dword_masked(HWIO_CD_TH_CD15_CPn_ADDR(n), HWIO_CD_TH_CD15_CPn_RMSK)
#define HWIO_CD_TH_CD15_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_CD_TH_CD15_CPn_ADDR(n), mask)
#define HWIO_CD_TH_CD15_CPn_OUTI(n,val)    \
        out_dword(HWIO_CD_TH_CD15_CPn_ADDR(n),val)
#define HWIO_CD_TH_CD15_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CD_TH_CD15_CPn_ADDR(n),mask,val,HWIO_CD_TH_CD15_CPn_INI(n))
#define HWIO_CD_TH_CD15_CPn_BW_BMSK                                 0x3fff
#define HWIO_CD_TH_CD15_CPn_BW_SHFT                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: BCM_SEQ_CFG
 *--------------------------------------------------------------------------*/

#define BCM_SEQ_CFG_REG_BASE                                                (AOSS_BASE      + 0x00be4000)

#define HWIO_SEQ_CFG_HW_START_ADDR_CDm_ADDR(m)                              (BCM_SEQ_CFG_REG_BASE      + 0x00000080 + 0x4 * (m))
#define HWIO_SEQ_CFG_HW_START_ADDR_CDm_RMSK                                       0xff
#define HWIO_SEQ_CFG_HW_START_ADDR_CDm_MAXm                                         15
#define HWIO_SEQ_CFG_HW_START_ADDR_CDm_INI(m)        \
        in_dword_masked(HWIO_SEQ_CFG_HW_START_ADDR_CDm_ADDR(m), HWIO_SEQ_CFG_HW_START_ADDR_CDm_RMSK)
#define HWIO_SEQ_CFG_HW_START_ADDR_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SEQ_CFG_HW_START_ADDR_CDm_ADDR(m), mask)
#define HWIO_SEQ_CFG_HW_START_ADDR_CDm_OUTI(m,val)    \
        out_dword(HWIO_SEQ_CFG_HW_START_ADDR_CDm_ADDR(m),val)
#define HWIO_SEQ_CFG_HW_START_ADDR_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SEQ_CFG_HW_START_ADDR_CDm_ADDR(m),mask,val,HWIO_SEQ_CFG_HW_START_ADDR_CDm_INI(m))
#define HWIO_SEQ_CFG_HW_START_ADDR_CDm_HW_START_ADDR_BMSK                         0xff
#define HWIO_SEQ_CFG_HW_START_ADDR_CDm_HW_START_ADDR_SHFT                          0x0

#define HWIO_SEQ_CFG_SW_START_ADDR_CDm_ADDR(m)                              (BCM_SEQ_CFG_REG_BASE      + 0x00000100 + 0x4 * (m))
#define HWIO_SEQ_CFG_SW_START_ADDR_CDm_RMSK                                       0xff
#define HWIO_SEQ_CFG_SW_START_ADDR_CDm_MAXm                                         15
#define HWIO_SEQ_CFG_SW_START_ADDR_CDm_INI(m)        \
        in_dword_masked(HWIO_SEQ_CFG_SW_START_ADDR_CDm_ADDR(m), HWIO_SEQ_CFG_SW_START_ADDR_CDm_RMSK)
#define HWIO_SEQ_CFG_SW_START_ADDR_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SEQ_CFG_SW_START_ADDR_CDm_ADDR(m), mask)
#define HWIO_SEQ_CFG_SW_START_ADDR_CDm_OUTI(m,val)    \
        out_dword(HWIO_SEQ_CFG_SW_START_ADDR_CDm_ADDR(m),val)
#define HWIO_SEQ_CFG_SW_START_ADDR_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SEQ_CFG_SW_START_ADDR_CDm_ADDR(m),mask,val,HWIO_SEQ_CFG_SW_START_ADDR_CDm_INI(m))
#define HWIO_SEQ_CFG_SW_START_ADDR_CDm_SW_START_ADDR_BMSK                         0xff
#define HWIO_SEQ_CFG_SW_START_ADDR_CDm_SW_START_ADDR_SHFT                          0x0

#define HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_ADDR(m,b)                          (BCM_SEQ_CFG_REG_BASE      + 0x00000180 + 0x40 * (m) + 0x4 * (b))
#define HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_RMSK                                     0xff
#define HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_MAXm                                       15
#define HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_MAXb                                       15
#define HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_INI2(m,b)        \
        in_dword_masked(HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_ADDR(m,b), HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_RMSK)
#define HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_INMI2(m,b,mask)    \
        in_dword_masked(HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_ADDR(m,b), mask)
#define HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_OUTI2(m,b,val)    \
        out_dword(HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_ADDR(m,b),val)
#define HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_OUTMI2(m,b,mask,val) \
        out_dword_masked_ns(HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_ADDR(m,b),mask,val,HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_INI2(m,b))
#define HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_BRANCH_ADDR_BMSK                         0xff
#define HWIO_SEQ_CFG_BRANCH_ADDR_CDm_BRb_BRANCH_ADDR_SHFT                          0x0

#define HWIO_SEQ_CFG_DELAY_CDm_DLYy_ADDR(m,y)                               (BCM_SEQ_CFG_REG_BASE      + 0x00000980 + 0x40 * (m) + 0x4 * (y))
#define HWIO_SEQ_CFG_DELAY_CDm_DLYy_RMSK                                       0xfffff
#define HWIO_SEQ_CFG_DELAY_CDm_DLYy_MAXm                                            15
#define HWIO_SEQ_CFG_DELAY_CDm_DLYy_MAXy                                             3
#define HWIO_SEQ_CFG_DELAY_CDm_DLYy_INI2(m,y)        \
        in_dword_masked(HWIO_SEQ_CFG_DELAY_CDm_DLYy_ADDR(m,y), HWIO_SEQ_CFG_DELAY_CDm_DLYy_RMSK)
#define HWIO_SEQ_CFG_DELAY_CDm_DLYy_INMI2(m,y,mask)    \
        in_dword_masked(HWIO_SEQ_CFG_DELAY_CDm_DLYy_ADDR(m,y), mask)
#define HWIO_SEQ_CFG_DELAY_CDm_DLYy_OUTI2(m,y,val)    \
        out_dword(HWIO_SEQ_CFG_DELAY_CDm_DLYy_ADDR(m,y),val)
#define HWIO_SEQ_CFG_DELAY_CDm_DLYy_OUTMI2(m,y,mask,val) \
        out_dword_masked_ns(HWIO_SEQ_CFG_DELAY_CDm_DLYy_ADDR(m,y),mask,val,HWIO_SEQ_CFG_DELAY_CDm_DLYy_INI2(m,y))
#define HWIO_SEQ_CFG_DELAY_CDm_DLYy_DELAY_VAL_BMSK                             0xfffff
#define HWIO_SEQ_CFG_DELAY_CDm_DLYy_DELAY_VAL_SHFT                                 0x0

/*----------------------------------------------------------------------------
 * MODULE: BCM_TCS_CFG
 *--------------------------------------------------------------------------*/

#define BCM_TCS_CFG_REG_BASE                                               (AOSS_BASE      + 0x00be5400)

#define HWIO_TCS_CFG_CDm_ADDR(m)                                           (BCM_TCS_CFG_REG_BASE      + 0x00000000 + 0x4 * (m))
#define HWIO_TCS_CFG_CDm_RMSK                                               0x7070707
#define HWIO_TCS_CFG_CDm_MAXm                                                      15
#define HWIO_TCS_CFG_CDm_INI(m)        \
        in_dword_masked(HWIO_TCS_CFG_CDm_ADDR(m), HWIO_TCS_CFG_CDm_RMSK)
#define HWIO_TCS_CFG_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_TCS_CFG_CDm_ADDR(m), mask)
#define HWIO_TCS_CFG_CDm_OUTI(m,val)    \
        out_dword(HWIO_TCS_CFG_CDm_ADDR(m),val)
#define HWIO_TCS_CFG_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_CDm_ADDR(m),mask,val,HWIO_TCS_CFG_CDm_INI(m))
#define HWIO_TCS_CFG_CDm_TCS_CMD_WAIT_BMSK                                  0x7000000
#define HWIO_TCS_CFG_CDm_TCS_CMD_WAIT_SHFT                                       0x18
#define HWIO_TCS_CFG_CDm_TCS_CMD_RR_CPUP_BMSK                                 0x70000
#define HWIO_TCS_CFG_CDm_TCS_CMD_RR_CPUP_SHFT                                    0x10
#define HWIO_TCS_CFG_CDm_TCS_CMD_RR_CPDOWN_BMSK                                 0x700
#define HWIO_TCS_CFG_CDm_TCS_CMD_RR_CPDOWN_SHFT                                   0x8
#define HWIO_TCS_CFG_CDm_TCS_CMD_EN_BMSK                                          0x7
#define HWIO_TCS_CFG_CDm_TCS_CMD_EN_SHFT                                          0x0

#define HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_ADDR(m,t)                          (BCM_TCS_CFG_REG_BASE      + 0x00000080 + 0xC * (m) + 0x4 * (t))
#define HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_RMSK                               0x80e0ffff
#define HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_MAXm                                       15
#define HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_MAXt                                        2
#define HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_INI2(m,t)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_ADDR(m,t), HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_RMSK)
#define HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_INMI2(m,t,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_ADDR(m,t), mask)
#define HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_OUTI2(m,t,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_ADDR(m,t),val)
#define HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_OUTMI2(m,t,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_ADDR(m,t),mask,val,HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_INI2(m,t))
#define HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_IS_VRM_VOTE_BMSK                   0x80000000
#define HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_IS_VRM_VOTE_SHFT                         0x1f
#define HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_SLV_ID_BMSK                          0xe00000
#define HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_SLV_ID_SHFT                              0x15
#define HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_OFFSET_BMSK                            0xffff
#define HWIO_TCS_CFG_VOTE_ADDR_CDm_CMDt_OFFSET_SHFT                               0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_ADDR(n)                             (BCM_TCS_CFG_REG_BASE      + 0x00000280 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_RMSK                                     0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_MAXn                                        15
#define HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_CMD2_DATA_BMSK                           0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_CMD2_DATA_SHFT                             0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_CMD1_DATA_BMSK                            0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_CMD1_DATA_SHFT                             0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_CMD0_DATA_BMSK                             0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD0_CPn_CMD0_DATA_SHFT                             0x0

#define HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_ADDR(m,n)                           (BCM_TCS_CFG_REG_BASE      + 0x00000280 + 0x4 * (n) + 0x40 * (m))
#define HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_RMSK                                     0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_MAXm                                        15
#define HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_MAXn                                        15
#define HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_INI2(m,n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_ADDR(m,n), HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_ADDR(m,n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_OUTI2(m,n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_ADDR(m,n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_OUTMI2(m,n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_ADDR(m,n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_INI2(m,n))
#define HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_CMD2_DATA_BMSK                           0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_CMD2_DATA_SHFT                             0x8
#define HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_CMD1_DATA_BMSK                            0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_CMD1_DATA_SHFT                             0x4
#define HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_CMD0_DATA_BMSK                             0xf
#define HWIO_TCS_CFG_VOTE_DATA_CDm_CPn_CMD0_DATA_SHFT                             0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_ADDR(n)                             (BCM_TCS_CFG_REG_BASE      + 0x000002c0 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_RMSK                                     0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_MAXn                                        15
#define HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_CMD2_DATA_BMSK                           0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_CMD2_DATA_SHFT                             0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_CMD1_DATA_BMSK                            0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_CMD1_DATA_SHFT                             0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_CMD0_DATA_BMSK                             0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD1_CPn_CMD0_DATA_SHFT                             0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_ADDR(n)                             (BCM_TCS_CFG_REG_BASE      + 0x00000300 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_RMSK                                     0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_MAXn                                         7
#define HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_CMD2_DATA_BMSK                           0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_CMD2_DATA_SHFT                             0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_CMD1_DATA_BMSK                            0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_CMD1_DATA_SHFT                             0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_CMD0_DATA_BMSK                             0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD2_CPn_CMD0_DATA_SHFT                             0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_ADDR(n)                             (BCM_TCS_CFG_REG_BASE      + 0x00000340 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_RMSK                                     0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_MAXn                                         7
#define HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_CMD2_DATA_BMSK                           0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_CMD2_DATA_SHFT                             0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_CMD1_DATA_BMSK                            0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_CMD1_DATA_SHFT                             0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_CMD0_DATA_BMSK                             0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD3_CPn_CMD0_DATA_SHFT                             0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_ADDR(n)                             (BCM_TCS_CFG_REG_BASE      + 0x00000380 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_RMSK                                     0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_MAXn                                         7
#define HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_CMD2_DATA_BMSK                           0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_CMD2_DATA_SHFT                             0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_CMD1_DATA_BMSK                            0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_CMD1_DATA_SHFT                             0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_CMD0_DATA_BMSK                             0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD4_CPn_CMD0_DATA_SHFT                             0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_ADDR(n)                             (BCM_TCS_CFG_REG_BASE      + 0x000003c0 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_RMSK                                     0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_MAXn                                         7
#define HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_CMD2_DATA_BMSK                           0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_CMD2_DATA_SHFT                             0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_CMD1_DATA_BMSK                            0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_CMD1_DATA_SHFT                             0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_CMD0_DATA_BMSK                             0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD5_CPn_CMD0_DATA_SHFT                             0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_ADDR(n)                             (BCM_TCS_CFG_REG_BASE      + 0x00000400 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_RMSK                                     0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_MAXn                                         7
#define HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_CMD2_DATA_BMSK                           0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_CMD2_DATA_SHFT                             0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_CMD1_DATA_BMSK                            0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_CMD1_DATA_SHFT                             0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_CMD0_DATA_BMSK                             0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD6_CPn_CMD0_DATA_SHFT                             0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_ADDR(n)                             (BCM_TCS_CFG_REG_BASE      + 0x00000440 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_RMSK                                     0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_MAXn                                         7
#define HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_CMD2_DATA_BMSK                           0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_CMD2_DATA_SHFT                             0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_CMD1_DATA_BMSK                            0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_CMD1_DATA_SHFT                             0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_CMD0_DATA_BMSK                             0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD7_CPn_CMD0_DATA_SHFT                             0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_ADDR(n)                             (BCM_TCS_CFG_REG_BASE      + 0x00000480 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_RMSK                                     0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_MAXn                                         7
#define HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_CMD2_DATA_BMSK                           0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_CMD2_DATA_SHFT                             0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_CMD1_DATA_BMSK                            0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_CMD1_DATA_SHFT                             0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_CMD0_DATA_BMSK                             0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD8_CPn_CMD0_DATA_SHFT                             0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_ADDR(n)                             (BCM_TCS_CFG_REG_BASE      + 0x000004c0 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_RMSK                                     0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_MAXn                                         7
#define HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_CMD2_DATA_BMSK                           0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_CMD2_DATA_SHFT                             0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_CMD1_DATA_BMSK                            0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_CMD1_DATA_SHFT                             0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_CMD0_DATA_BMSK                             0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD9_CPn_CMD0_DATA_SHFT                             0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_ADDR(n)                            (BCM_TCS_CFG_REG_BASE      + 0x00000500 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_RMSK                                    0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_MAXn                                       15
#define HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_CMD2_DATA_BMSK                          0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_CMD2_DATA_SHFT                            0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_CMD1_DATA_BMSK                           0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_CMD1_DATA_SHFT                            0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_CMD0_DATA_BMSK                            0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD10_CPn_CMD0_DATA_SHFT                            0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_ADDR(n)                            (BCM_TCS_CFG_REG_BASE      + 0x00000540 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_RMSK                                    0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_MAXn                                       15
#define HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_CMD2_DATA_BMSK                          0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_CMD2_DATA_SHFT                            0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_CMD1_DATA_BMSK                           0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_CMD1_DATA_SHFT                            0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_CMD0_DATA_BMSK                            0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD11_CPn_CMD0_DATA_SHFT                            0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_ADDR(n)                            (BCM_TCS_CFG_REG_BASE      + 0x00000580 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_RMSK                                    0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_MAXn                                        7
#define HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_CMD2_DATA_BMSK                          0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_CMD2_DATA_SHFT                            0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_CMD1_DATA_BMSK                           0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_CMD1_DATA_SHFT                            0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_CMD0_DATA_BMSK                            0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD12_CPn_CMD0_DATA_SHFT                            0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_ADDR(n)                            (BCM_TCS_CFG_REG_BASE      + 0x000005c0 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_RMSK                                    0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_MAXn                                        7
#define HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_CMD2_DATA_BMSK                          0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_CMD2_DATA_SHFT                            0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_CMD1_DATA_BMSK                           0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_CMD1_DATA_SHFT                            0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_CMD0_DATA_BMSK                            0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD13_CPn_CMD0_DATA_SHFT                            0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_ADDR(n)                            (BCM_TCS_CFG_REG_BASE      + 0x00000600 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_RMSK                                    0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_MAXn                                       15
#define HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_CMD2_DATA_BMSK                          0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_CMD2_DATA_SHFT                            0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_CMD1_DATA_BMSK                           0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_CMD1_DATA_SHFT                            0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_CMD0_DATA_BMSK                            0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD14_CPn_CMD0_DATA_SHFT                            0x0

#define HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_ADDR(n)                            (BCM_TCS_CFG_REG_BASE      + 0x00000640 + 0x4 * (n))
#define HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_RMSK                                    0xfff
#define HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_MAXn                                       15
#define HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_INI(n)        \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_ADDR(n), HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_RMSK)
#define HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_INMI(n,mask)    \
        in_dword_masked(HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_ADDR(n), mask)
#define HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_OUTI(n,val)    \
        out_dword(HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_ADDR(n),val)
#define HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_ADDR(n),mask,val,HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_INI(n))
#define HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_CMD2_DATA_BMSK                          0xf00
#define HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_CMD2_DATA_SHFT                            0x8
#define HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_CMD1_DATA_BMSK                           0xf0
#define HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_CMD1_DATA_SHFT                            0x4
#define HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_CMD0_DATA_BMSK                            0xf
#define HWIO_TCS_CFG_VOTE_DATA_CD15_CPn_CMD0_DATA_SHFT                            0x0

#define HWIO_TCS_VOTE_RESP_BACK_ADDR                                       (BCM_TCS_CFG_REG_BASE      + 0x00000a80)
#define HWIO_TCS_VOTE_RESP_BACK_RMSK                                       0xffffffff
#define HWIO_TCS_VOTE_RESP_BACK_IN          \
        in_dword_masked(HWIO_TCS_VOTE_RESP_BACK_ADDR, HWIO_TCS_VOTE_RESP_BACK_RMSK)
#define HWIO_TCS_VOTE_RESP_BACK_INM(m)      \
        in_dword_masked(HWIO_TCS_VOTE_RESP_BACK_ADDR, m)
#define HWIO_TCS_VOTE_RESP_BACK_OUT(v)      \
        out_dword(HWIO_TCS_VOTE_RESP_BACK_ADDR,v)
#define HWIO_TCS_VOTE_RESP_BACK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCS_VOTE_RESP_BACK_ADDR,m,v,HWIO_TCS_VOTE_RESP_BACK_IN)
#define HWIO_TCS_VOTE_RESP_BACK_VOTE_RESP_BMSK                             0xffffffff
#define HWIO_TCS_VOTE_RESP_BACK_VOTE_RESP_SHFT                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: BCM_AU_CFG
 *--------------------------------------------------------------------------*/

#define BCM_AU_CFG_REG_BASE                                       (AOSS_BASE      + 0x00be6800)

#define HWIO_ALGO_ADDRg_ADDR(g)                                   (BCM_AU_CFG_REG_BASE      + 0x00000000 + 0x4 * (g))
#define HWIO_ALGO_ADDRg_RMSK                                            0xff
#define HWIO_ALGO_ADDRg_MAXg                                              31
#define HWIO_ALGO_ADDRg_INI(g)        \
        in_dword_masked(HWIO_ALGO_ADDRg_ADDR(g), HWIO_ALGO_ADDRg_RMSK)
#define HWIO_ALGO_ADDRg_INMI(g,mask)    \
        in_dword_masked(HWIO_ALGO_ADDRg_ADDR(g), mask)
#define HWIO_ALGO_ADDRg_OUTI(g,val)    \
        out_dword(HWIO_ALGO_ADDRg_ADDR(g),val)
#define HWIO_ALGO_ADDRg_OUTMI(g,mask,val) \
        out_dword_masked_ns(HWIO_ALGO_ADDRg_ADDR(g),mask,val,HWIO_ALGO_ADDRg_INI(g))
#define HWIO_ALGO_ADDRg_ALGO_ADDR_BMSK                                  0xff
#define HWIO_ALGO_ADDRg_ALGO_ADDR_SHFT                                   0x0

#define HWIO_AU_CFG_SCALARk_ADDR(k)                               (BCM_AU_CFG_REG_BASE      + 0x00000100 + 0x4 * (k))
#define HWIO_AU_CFG_SCALARk_RMSK                                        0xff
#define HWIO_AU_CFG_SCALARk_MAXk                                          31
#define HWIO_AU_CFG_SCALARk_INI(k)        \
        in_dword_masked(HWIO_AU_CFG_SCALARk_ADDR(k), HWIO_AU_CFG_SCALARk_RMSK)
#define HWIO_AU_CFG_SCALARk_INMI(k,mask)    \
        in_dword_masked(HWIO_AU_CFG_SCALARk_ADDR(k), mask)
#define HWIO_AU_CFG_SCALARk_OUTI(k,val)    \
        out_dword(HWIO_AU_CFG_SCALARk_ADDR(k),val)
#define HWIO_AU_CFG_SCALARk_OUTMI(k,mask,val) \
        out_dword_masked_ns(HWIO_AU_CFG_SCALARk_ADDR(k),mask,val,HWIO_AU_CFG_SCALARk_INI(k))
#define HWIO_AU_CFG_SCALARk_VALUE_BMSK                                  0xff
#define HWIO_AU_CFG_SCALARk_VALUE_SHFT                                   0x0

#define HWIO_AU_CFG_SCALAR_FORMAT_ADDR                            (BCM_AU_CFG_REG_BASE      + 0x00000180)
#define HWIO_AU_CFG_SCALAR_FORMAT_RMSK                                   0x7
#define HWIO_AU_CFG_SCALAR_FORMAT_IN          \
        in_dword_masked(HWIO_AU_CFG_SCALAR_FORMAT_ADDR, HWIO_AU_CFG_SCALAR_FORMAT_RMSK)
#define HWIO_AU_CFG_SCALAR_FORMAT_INM(m)      \
        in_dword_masked(HWIO_AU_CFG_SCALAR_FORMAT_ADDR, m)
#define HWIO_AU_CFG_SCALAR_FORMAT_OUT(v)      \
        out_dword(HWIO_AU_CFG_SCALAR_FORMAT_ADDR,v)
#define HWIO_AU_CFG_SCALAR_FORMAT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AU_CFG_SCALAR_FORMAT_ADDR,m,v,HWIO_AU_CFG_SCALAR_FORMAT_IN)
#define HWIO_AU_CFG_SCALAR_FORMAT_FRACTION_BMSK                          0x7
#define HWIO_AU_CFG_SCALAR_FORMAT_FRACTION_SHFT                          0x0

/*----------------------------------------------------------------------------
 * MODULE: BCM_CFG
 *--------------------------------------------------------------------------*/

#define BCM_CFG_REG_BASE                                                   (AOSS_BASE      + 0x00be7000)

#define HWIO_BCM_CFG_BCM_EN_ADDR                                           (BCM_CFG_REG_BASE      + 0x00000000)
#define HWIO_BCM_CFG_BCM_EN_RMSK                                                  0x1
#define HWIO_BCM_CFG_BCM_EN_IN          \
        in_dword_masked(HWIO_BCM_CFG_BCM_EN_ADDR, HWIO_BCM_CFG_BCM_EN_RMSK)
#define HWIO_BCM_CFG_BCM_EN_INM(m)      \
        in_dword_masked(HWIO_BCM_CFG_BCM_EN_ADDR, m)
#define HWIO_BCM_CFG_BCM_EN_OUT(v)      \
        out_dword(HWIO_BCM_CFG_BCM_EN_ADDR,v)
#define HWIO_BCM_CFG_BCM_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BCM_CFG_BCM_EN_ADDR,m,v,HWIO_BCM_CFG_BCM_EN_IN)
#define HWIO_BCM_CFG_BCM_EN_BCM_EN_BMSK                                           0x1
#define HWIO_BCM_CFG_BCM_EN_BCM_EN_SHFT                                           0x0

#define HWIO_BCM_CFG_SLV_ID_FOR_RESP_ADDR                                  (BCM_CFG_REG_BASE      + 0x00000004)
#define HWIO_BCM_CFG_SLV_ID_FOR_RESP_RMSK                                         0x7
#define HWIO_BCM_CFG_SLV_ID_FOR_RESP_IN          \
        in_dword_masked(HWIO_BCM_CFG_SLV_ID_FOR_RESP_ADDR, HWIO_BCM_CFG_SLV_ID_FOR_RESP_RMSK)
#define HWIO_BCM_CFG_SLV_ID_FOR_RESP_INM(m)      \
        in_dword_masked(HWIO_BCM_CFG_SLV_ID_FOR_RESP_ADDR, m)
#define HWIO_BCM_CFG_SLV_ID_FOR_RESP_OUT(v)      \
        out_dword(HWIO_BCM_CFG_SLV_ID_FOR_RESP_ADDR,v)
#define HWIO_BCM_CFG_SLV_ID_FOR_RESP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BCM_CFG_SLV_ID_FOR_RESP_ADDR,m,v,HWIO_BCM_CFG_SLV_ID_FOR_RESP_IN)
#define HWIO_BCM_CFG_SLV_ID_FOR_RESP_SLV_ID_BMSK                                  0x7
#define HWIO_BCM_CFG_SLV_ID_FOR_RESP_SLV_ID_SHFT                                  0x0

#define HWIO_BCM_CFG_DRV_ID_FOR_ARC_VOTE_CDm_ADDR(m)                       (BCM_CFG_REG_BASE      + 0x00000008 + 0x4 * (m))
#define HWIO_BCM_CFG_DRV_ID_FOR_ARC_VOTE_CDm_RMSK                                0x1f
#define HWIO_BCM_CFG_DRV_ID_FOR_ARC_VOTE_CDm_MAXm                                  15
#define HWIO_BCM_CFG_DRV_ID_FOR_ARC_VOTE_CDm_INI(m)        \
        in_dword_masked(HWIO_BCM_CFG_DRV_ID_FOR_ARC_VOTE_CDm_ADDR(m), HWIO_BCM_CFG_DRV_ID_FOR_ARC_VOTE_CDm_RMSK)
#define HWIO_BCM_CFG_DRV_ID_FOR_ARC_VOTE_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_BCM_CFG_DRV_ID_FOR_ARC_VOTE_CDm_ADDR(m), mask)
#define HWIO_BCM_CFG_DRV_ID_FOR_ARC_VOTE_CDm_OUTI(m,val)    \
        out_dword(HWIO_BCM_CFG_DRV_ID_FOR_ARC_VOTE_CDm_ADDR(m),val)
#define HWIO_BCM_CFG_DRV_ID_FOR_ARC_VOTE_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_BCM_CFG_DRV_ID_FOR_ARC_VOTE_CDm_ADDR(m),mask,val,HWIO_BCM_CFG_DRV_ID_FOR_ARC_VOTE_CDm_INI(m))
#define HWIO_BCM_CFG_DRV_ID_FOR_ARC_VOTE_CDm_DRV_ID_BMSK                         0x1f
#define HWIO_BCM_CFG_DRV_ID_FOR_ARC_VOTE_CDm_DRV_ID_SHFT                          0x0

#define HWIO_BCM_CFG_DRV_ID_FOR_VRM_VOTE_ADDR                              (BCM_CFG_REG_BASE      + 0x00000088)
#define HWIO_BCM_CFG_DRV_ID_FOR_VRM_VOTE_RMSK                                    0x1f
#define HWIO_BCM_CFG_DRV_ID_FOR_VRM_VOTE_IN          \
        in_dword_masked(HWIO_BCM_CFG_DRV_ID_FOR_VRM_VOTE_ADDR, HWIO_BCM_CFG_DRV_ID_FOR_VRM_VOTE_RMSK)
#define HWIO_BCM_CFG_DRV_ID_FOR_VRM_VOTE_INM(m)      \
        in_dword_masked(HWIO_BCM_CFG_DRV_ID_FOR_VRM_VOTE_ADDR, m)
#define HWIO_BCM_CFG_DRV_ID_FOR_VRM_VOTE_OUT(v)      \
        out_dword(HWIO_BCM_CFG_DRV_ID_FOR_VRM_VOTE_ADDR,v)
#define HWIO_BCM_CFG_DRV_ID_FOR_VRM_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BCM_CFG_DRV_ID_FOR_VRM_VOTE_ADDR,m,v,HWIO_BCM_CFG_DRV_ID_FOR_VRM_VOTE_IN)
#define HWIO_BCM_CFG_DRV_ID_FOR_VRM_VOTE_DRV_ID_BMSK                             0x1f
#define HWIO_BCM_CFG_DRV_ID_FOR_VRM_VOTE_DRV_ID_SHFT                              0x0

#define HWIO_BCM_CFG_DRVr_ADDR(r)                                          (BCM_CFG_REG_BASE      + 0x0000008c + 0x4 * (r))
#define HWIO_BCM_CFG_DRVr_RMSK                                               0xffffff
#define HWIO_BCM_CFG_DRVr_MAXr                                                     11
#define HWIO_BCM_CFG_DRVr_INI(r)        \
        in_dword_masked(HWIO_BCM_CFG_DRVr_ADDR(r), HWIO_BCM_CFG_DRVr_RMSK)
#define HWIO_BCM_CFG_DRVr_INMI(r,mask)    \
        in_dword_masked(HWIO_BCM_CFG_DRVr_ADDR(r), mask)
#define HWIO_BCM_CFG_DRVr_OUTI(r,val)    \
        out_dword(HWIO_BCM_CFG_DRVr_ADDR(r),val)
#define HWIO_BCM_CFG_DRVr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_BCM_CFG_DRVr_ADDR(r),mask,val,HWIO_BCM_CFG_DRVr_INI(r))
#define HWIO_BCM_CFG_DRVr_RESP_ADDR_BMSK                                     0xffffff
#define HWIO_BCM_CFG_DRVr_RESP_ADDR_SHFT                                          0x0

#define HWIO_BCM_CFG_DEBUG_BUS_ADDR                                        (BCM_CFG_REG_BASE      + 0x0000010c)
#define HWIO_BCM_CFG_DEBUG_BUS_RMSK                                             0x10f
#define HWIO_BCM_CFG_DEBUG_BUS_IN          \
        in_dword_masked(HWIO_BCM_CFG_DEBUG_BUS_ADDR, HWIO_BCM_CFG_DEBUG_BUS_RMSK)
#define HWIO_BCM_CFG_DEBUG_BUS_INM(m)      \
        in_dword_masked(HWIO_BCM_CFG_DEBUG_BUS_ADDR, m)
#define HWIO_BCM_CFG_DEBUG_BUS_OUT(v)      \
        out_dword(HWIO_BCM_CFG_DEBUG_BUS_ADDR,v)
#define HWIO_BCM_CFG_DEBUG_BUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BCM_CFG_DEBUG_BUS_ADDR,m,v,HWIO_BCM_CFG_DEBUG_BUS_IN)
#define HWIO_BCM_CFG_DEBUG_BUS_DEBUG_BUS_EN_BMSK                                0x100
#define HWIO_BCM_CFG_DEBUG_BUS_DEBUG_BUS_EN_SHFT                                  0x8
#define HWIO_BCM_CFG_DEBUG_BUS_CD_SEL_BMSK                                        0xf
#define HWIO_BCM_CFG_DEBUG_BUS_CD_SEL_SHFT                                        0x0

#define HWIO_BCM_CFG_ARCH_CG_DISABLE_ADDR                                  (BCM_CFG_REG_BASE      + 0x00000110)
#define HWIO_BCM_CFG_ARCH_CG_DISABLE_RMSK                                         0x3
#define HWIO_BCM_CFG_ARCH_CG_DISABLE_IN          \
        in_dword_masked(HWIO_BCM_CFG_ARCH_CG_DISABLE_ADDR, HWIO_BCM_CFG_ARCH_CG_DISABLE_RMSK)
#define HWIO_BCM_CFG_ARCH_CG_DISABLE_INM(m)      \
        in_dword_masked(HWIO_BCM_CFG_ARCH_CG_DISABLE_ADDR, m)
#define HWIO_BCM_CFG_ARCH_CG_DISABLE_OUT(v)      \
        out_dword(HWIO_BCM_CFG_ARCH_CG_DISABLE_ADDR,v)
#define HWIO_BCM_CFG_ARCH_CG_DISABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BCM_CFG_ARCH_CG_DISABLE_ADDR,m,v,HWIO_BCM_CFG_ARCH_CG_DISABLE_IN)
#define HWIO_BCM_CFG_ARCH_CG_DISABLE_BCM_FE_BMSK                                  0x2
#define HWIO_BCM_CFG_ARCH_CG_DISABLE_BCM_FE_SHFT                                  0x1
#define HWIO_BCM_CFG_ARCH_CG_DISABLE_BCM_CE_BMSK                                  0x1
#define HWIO_BCM_CFG_ARCH_CG_DISABLE_BCM_CE_SHFT                                  0x0

/*----------------------------------------------------------------------------
 * MODULE: BCM_STATUS
 *--------------------------------------------------------------------------*/

#define BCM_STATUS_REG_BASE                                                     (AOSS_BASE      + 0x00bf0000)

#define HWIO_STATUS_FE_AGG_FSM_ADDR                                             (BCM_STATUS_REG_BASE      + 0x00000000)
#define HWIO_STATUS_FE_AGG_FSM_RMSK                                                 0xffff
#define HWIO_STATUS_FE_AGG_FSM_IN          \
        in_dword_masked(HWIO_STATUS_FE_AGG_FSM_ADDR, HWIO_STATUS_FE_AGG_FSM_RMSK)
#define HWIO_STATUS_FE_AGG_FSM_INM(m)      \
        in_dword_masked(HWIO_STATUS_FE_AGG_FSM_ADDR, m)
#define HWIO_STATUS_FE_AGG_FSM_AGG_BUSY_CD_BMSK                                     0xffff
#define HWIO_STATUS_FE_AGG_FSM_AGG_BUSY_CD_SHFT                                        0x0

#define HWIO_STATUS_FE_CDm_ADDR(m)                                              (BCM_STATUS_REG_BASE      + 0x00000004 + 0x4 * (m))
#define HWIO_STATUS_FE_CDm_RMSK                                                 0x7fff10ff
#define HWIO_STATUS_FE_CDm_MAXm                                                         15
#define HWIO_STATUS_FE_CDm_INI(m)        \
        in_dword_masked(HWIO_STATUS_FE_CDm_ADDR(m), HWIO_STATUS_FE_CDm_RMSK)
#define HWIO_STATUS_FE_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_STATUS_FE_CDm_ADDR(m), mask)
#define HWIO_STATUS_FE_CDm_AGG_BW_BMSK                                          0x7fff0000
#define HWIO_STATUS_FE_CDm_AGG_BW_SHFT                                                0x10
#define HWIO_STATUS_FE_CDm_COMMIT_CD_RAW_BMSK                                       0x1000
#define HWIO_STATUS_FE_CDm_COMMIT_CD_RAW_SHFT                                          0xc
#define HWIO_STATUS_FE_CDm_FINAL_CP_BMSK                                              0xf0
#define HWIO_STATUS_FE_CDm_FINAL_CP_SHFT                                               0x4
#define HWIO_STATUS_FE_CDm_AGG_CP_BMSK                                                 0xf
#define HWIO_STATUS_FE_CDm_AGG_CP_SHFT                                                 0x0

#define HWIO_STATUS_DIRTY_ND_0_ADDR                                             (BCM_STATUS_REG_BASE      + 0x00000084)
#define HWIO_STATUS_DIRTY_ND_0_RMSK                                             0xffffffff
#define HWIO_STATUS_DIRTY_ND_0_IN          \
        in_dword_masked(HWIO_STATUS_DIRTY_ND_0_ADDR, HWIO_STATUS_DIRTY_ND_0_RMSK)
#define HWIO_STATUS_DIRTY_ND_0_INM(m)      \
        in_dword_masked(HWIO_STATUS_DIRTY_ND_0_ADDR, m)
#define HWIO_STATUS_DIRTY_ND_0_DIRTY_ND_31_0_BMSK                               0xffffffff
#define HWIO_STATUS_DIRTY_ND_0_DIRTY_ND_31_0_SHFT                                      0x0

#define HWIO_STATUS_DIRTY_ND_1_ADDR                                             (BCM_STATUS_REG_BASE      + 0x00000088)
#define HWIO_STATUS_DIRTY_ND_1_RMSK                                             0xffffffff
#define HWIO_STATUS_DIRTY_ND_1_IN          \
        in_dword_masked(HWIO_STATUS_DIRTY_ND_1_ADDR, HWIO_STATUS_DIRTY_ND_1_RMSK)
#define HWIO_STATUS_DIRTY_ND_1_INM(m)      \
        in_dword_masked(HWIO_STATUS_DIRTY_ND_1_ADDR, m)
#define HWIO_STATUS_DIRTY_ND_1_DIRTY_ND_63_32_BMSK                              0xffffffff
#define HWIO_STATUS_DIRTY_ND_1_DIRTY_ND_63_32_SHFT                                     0x0

#define HWIO_STATUS_SNDs_ADDR(s)                                                (BCM_STATUS_REG_BASE      + 0x0000008c + 0x4 * (s))
#define HWIO_STATUS_SNDs_RMSK                                                          0xf
#define HWIO_STATUS_SNDs_MAXs                                                            7
#define HWIO_STATUS_SNDs_INI(s)        \
        in_dword_masked(HWIO_STATUS_SNDs_ADDR(s), HWIO_STATUS_SNDs_RMSK)
#define HWIO_STATUS_SNDs_INMI(s,mask)    \
        in_dword_masked(HWIO_STATUS_SNDs_ADDR(s), mask)
#define HWIO_STATUS_SNDs_SEL_CP_BMSK                                                   0xf
#define HWIO_STATUS_SNDs_SEL_CP_SHFT                                                   0x0

#define HWIO_STATUS_AGG_BW_NDd_ADDR(d)                                          (BCM_STATUS_REG_BASE      + 0x0000010c + 0x4 * (d))
#define HWIO_STATUS_AGG_BW_NDd_RMSK                                                 0x7fff
#define HWIO_STATUS_AGG_BW_NDd_MAXd                                                     63
#define HWIO_STATUS_AGG_BW_NDd_INI(d)        \
        in_dword_masked(HWIO_STATUS_AGG_BW_NDd_ADDR(d), HWIO_STATUS_AGG_BW_NDd_RMSK)
#define HWIO_STATUS_AGG_BW_NDd_INMI(d,mask)    \
        in_dword_masked(HWIO_STATUS_AGG_BW_NDd_ADDR(d), mask)
#define HWIO_STATUS_AGG_BW_NDd_AGG_BW_BMSK                                          0x7fff
#define HWIO_STATUS_AGG_BW_NDd_AGG_BW_SHFT                                             0x0

#define HWIO_STATUS_AGG_BW_Z_NDs_ADDR(s)                                        (BCM_STATUS_REG_BASE      + 0x0000030c + 0x4 * (s))
#define HWIO_STATUS_AGG_BW_Z_NDs_RMSK                                               0x7fff
#define HWIO_STATUS_AGG_BW_Z_NDs_MAXs                                                    7
#define HWIO_STATUS_AGG_BW_Z_NDs_INI(s)        \
        in_dword_masked(HWIO_STATUS_AGG_BW_Z_NDs_ADDR(s), HWIO_STATUS_AGG_BW_Z_NDs_RMSK)
#define HWIO_STATUS_AGG_BW_Z_NDs_INMI(s,mask)    \
        in_dword_masked(HWIO_STATUS_AGG_BW_Z_NDs_ADDR(s), mask)
#define HWIO_STATUS_AGG_BW_Z_NDs_AGG_BW_Z_BMSK                                      0x7fff
#define HWIO_STATUS_AGG_BW_Z_NDs_AGG_BW_Z_SHFT                                         0x0

#define HWIO_STATUS_BE_CDm_ADDR(m)                                              (BCM_STATUS_REG_BASE      + 0x00000800 + 0x4 * (m))
#define HWIO_STATUS_BE_CDm_RMSK                                                    0xfffff
#define HWIO_STATUS_BE_CDm_MAXm                                                         15
#define HWIO_STATUS_BE_CDm_INI(m)        \
        in_dword_masked(HWIO_STATUS_BE_CDm_ADDR(m), HWIO_STATUS_BE_CDm_RMSK)
#define HWIO_STATUS_BE_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_STATUS_BE_CDm_ADDR(m), mask)
#define HWIO_STATUS_BE_CDm_CLK_DEST_STATE_BMSK                                     0xf0000
#define HWIO_STATUS_BE_CDm_CLK_DEST_STATE_SHFT                                        0x10
#define HWIO_STATUS_BE_CDm_COMBINED_CP_BMSK                                         0xf000
#define HWIO_STATUS_BE_CDm_COMBINED_CP_SHFT                                            0xc
#define HWIO_STATUS_BE_CDm_SW_CP_SNAP_BMSK                                           0xf00
#define HWIO_STATUS_BE_CDm_SW_CP_SNAP_SHFT                                             0x8
#define HWIO_STATUS_BE_CDm_WRITTEN_CP_BMSK                                            0xf0
#define HWIO_STATUS_BE_CDm_WRITTEN_CP_SHFT                                             0x4
#define HWIO_STATUS_BE_CDm_CURR_CP_BMSK                                                0xf
#define HWIO_STATUS_BE_CDm_CURR_CP_SHFT                                                0x0

#define HWIO_STATUS_BE_SEQ_CDm_ADDR(m)                                          (BCM_STATUS_REG_BASE      + 0x00000880 + 0x4 * (m))
#define HWIO_STATUS_BE_SEQ_CDm_RMSK                                                 0xff01
#define HWIO_STATUS_BE_SEQ_CDm_MAXm                                                     15
#define HWIO_STATUS_BE_SEQ_CDm_INI(m)        \
        in_dword_masked(HWIO_STATUS_BE_SEQ_CDm_ADDR(m), HWIO_STATUS_BE_SEQ_CDm_RMSK)
#define HWIO_STATUS_BE_SEQ_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_STATUS_BE_SEQ_CDm_ADDR(m), mask)
#define HWIO_STATUS_BE_SEQ_CDm_SEQ_CURR_PC_BMSK                                     0xff00
#define HWIO_STATUS_BE_SEQ_CDm_SEQ_CURR_PC_SHFT                                        0x8
#define HWIO_STATUS_BE_SEQ_CDm_SEQ_STATE_BMSK                                          0x1
#define HWIO_STATUS_BE_SEQ_CDm_SEQ_STATE_SHFT                                          0x0

#define HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_ADDR(m)                             (BCM_STATUS_REG_BASE      + 0x00000900 + 0x4 * (m))
#define HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_RMSK                                       0x7
#define HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_MAXm                                        15
#define HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_INI(m)        \
        in_dword_masked(HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_ADDR(m), HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_RMSK)
#define HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_ADDR(m), mask)
#define HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_ISSUE_STATUS_BMSK                          0x7
#define HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_ISSUE_STATUS_SHFT                          0x0

#define HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_ADDR(m)                              (BCM_STATUS_REG_BASE      + 0x00000980 + 0x4 * (m))
#define HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_RMSK                                        0x7
#define HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_MAXm                                         15
#define HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_INI(m)        \
        in_dword_masked(HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_ADDR(m), HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_RMSK)
#define HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_ADDR(m), mask)
#define HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_SENT_STATUS_BMSK                            0x7
#define HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_SENT_STATUS_SHFT                            0x0

#define HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_ADDR(m)                              (BCM_STATUS_REG_BASE      + 0x00000a00 + 0x4 * (m))
#define HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_RMSK                                        0x7
#define HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_MAXm                                         15
#define HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_INI(m)        \
        in_dword_masked(HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_ADDR(m), HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_RMSK)
#define HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_ADDR(m), mask)
#define HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_CMPL_STATUS_BMSK                            0x7
#define HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_CMPL_STATUS_SHFT                            0x0

#define HWIO_STATUS_BE_TCS_STATUS_CDm_ADDR(m)                                   (BCM_STATUS_REG_BASE      + 0x00000a80 + 0x4 * (m))
#define HWIO_STATUS_BE_TCS_STATUS_CDm_RMSK                                             0x7
#define HWIO_STATUS_BE_TCS_STATUS_CDm_MAXm                                              15
#define HWIO_STATUS_BE_TCS_STATUS_CDm_INI(m)        \
        in_dword_masked(HWIO_STATUS_BE_TCS_STATUS_CDm_ADDR(m), HWIO_STATUS_BE_TCS_STATUS_CDm_RMSK)
#define HWIO_STATUS_BE_TCS_STATUS_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_STATUS_BE_TCS_STATUS_CDm_ADDR(m), mask)
#define HWIO_STATUS_BE_TCS_STATUS_CDm_TCS_CURR_STATE_BMSK                              0x7
#define HWIO_STATUS_BE_TCS_STATUS_CDm_TCS_CURR_STATE_SHFT                              0x0

#define HWIO_STATUS_CE_DRVr_CTTe_ADDR(r,e)                                      (BCM_STATUS_REG_BASE      + 0x00000f00 + 0x60 * (r) + 0x4 * (e))
#define HWIO_STATUS_CE_DRVr_CTTe_RMSK                                           0xffff07ff
#define HWIO_STATUS_CE_DRVr_CTTe_MAXr                                                   11
#define HWIO_STATUS_CE_DRVr_CTTe_MAXe                                                   23
#define HWIO_STATUS_CE_DRVr_CTTe_INI2(r,e)        \
        in_dword_masked(HWIO_STATUS_CE_DRVr_CTTe_ADDR(r,e), HWIO_STATUS_CE_DRVr_CTTe_RMSK)
#define HWIO_STATUS_CE_DRVr_CTTe_INMI2(r,e,mask)    \
        in_dword_masked(HWIO_STATUS_CE_DRVr_CTTe_ADDR(r,e), mask)
#define HWIO_STATUS_CE_DRVr_CTTe_OFFSET_BMSK                                    0xffff0000
#define HWIO_STATUS_CE_DRVr_CTTe_OFFSET_SHFT                                          0x10
#define HWIO_STATUS_CE_DRVr_CTTe_DESIRED_CP_BMSK                                     0x780
#define HWIO_STATUS_CE_DRVr_CTTe_DESIRED_CP_SHFT                                       0x7
#define HWIO_STATUS_CE_DRVr_CTTe_CD_ID_BMSK                                           0x78
#define HWIO_STATUS_CE_DRVr_CTTe_CD_ID_SHFT                                            0x3
#define HWIO_STATUS_CE_DRVr_CTTe_CTT_STATUS_BMSK                                       0x7
#define HWIO_STATUS_CE_DRVr_CTTe_CTT_STATUS_SHFT                                       0x0

#define HWIO_STATUS_CE_ADDR                                                     (BCM_STATUS_REG_BASE      + 0x00001700)
#define HWIO_STATUS_CE_RMSK                                                        0xfff01
#define HWIO_STATUS_CE_IN          \
        in_dword_masked(HWIO_STATUS_CE_ADDR, HWIO_STATUS_CE_RMSK)
#define HWIO_STATUS_CE_INM(m)      \
        in_dword_masked(HWIO_STATUS_CE_ADDR, m)
#define HWIO_STATUS_CE_CTT_EMPTY_DRV_BMSK                                          0xfff00
#define HWIO_STATUS_CE_CTT_EMPTY_DRV_SHFT                                              0x8
#define HWIO_STATUS_CE_CTT_EMPTY_BMSK                                                  0x1
#define HWIO_STATUS_CE_CTT_EMPTY_SHFT                                                  0x0

#define HWIO_STATUS_BCM_ADDR                                                    (BCM_STATUS_REG_BASE      + 0x00001704)
#define HWIO_STATUS_BCM_RMSK                                                           0x1
#define HWIO_STATUS_BCM_IN          \
        in_dword_masked(HWIO_STATUS_BCM_ADDR, HWIO_STATUS_BCM_RMSK)
#define HWIO_STATUS_BCM_INM(m)      \
        in_dword_masked(HWIO_STATUS_BCM_ADDR, m)
#define HWIO_STATUS_BCM_BCM_IDLE_BMSK                                                  0x1
#define HWIO_STATUS_BCM_BCM_IDLE_SHFT                                                  0x0

#define HWIO_HW_VERSION_ADDR                                                    (BCM_STATUS_REG_BASE      + 0x00001708)
#define HWIO_HW_VERSION_RMSK                                                      0xffffff
#define HWIO_HW_VERSION_IN          \
        in_dword_masked(HWIO_HW_VERSION_ADDR, HWIO_HW_VERSION_RMSK)
#define HWIO_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_HW_VERSION_ADDR, m)
#define HWIO_HW_VERSION_MAJOR_BMSK                                                0xff0000
#define HWIO_HW_VERSION_MAJOR_SHFT                                                    0x10
#define HWIO_HW_VERSION_MINOR_BMSK                                                  0xff00
#define HWIO_HW_VERSION_MINOR_SHFT                                                     0x8
#define HWIO_HW_VERSION_STEP_BMSK                                                     0xff
#define HWIO_HW_VERSION_STEP_SHFT                                                      0x0

/*----------------------------------------------------------------------------
 * MODULE: BCM_INTERRUPT
 *--------------------------------------------------------------------------*/

#define BCM_INTERRUPT_REG_BASE                                                 (AOSS_BASE      + 0x00bf1f00)

#define HWIO_INTERRUPT_AGG_DONE_INT_EN_ADDR                                    (BCM_INTERRUPT_REG_BASE      + 0x00000000)
#define HWIO_INTERRUPT_AGG_DONE_INT_EN_RMSK                                        0xffff
#define HWIO_INTERRUPT_AGG_DONE_INT_EN_IN          \
        in_dword_masked(HWIO_INTERRUPT_AGG_DONE_INT_EN_ADDR, HWIO_INTERRUPT_AGG_DONE_INT_EN_RMSK)
#define HWIO_INTERRUPT_AGG_DONE_INT_EN_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_AGG_DONE_INT_EN_ADDR, m)
#define HWIO_INTERRUPT_AGG_DONE_INT_EN_OUT(v)      \
        out_dword(HWIO_INTERRUPT_AGG_DONE_INT_EN_ADDR,v)
#define HWIO_INTERRUPT_AGG_DONE_INT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_INTERRUPT_AGG_DONE_INT_EN_ADDR,m,v,HWIO_INTERRUPT_AGG_DONE_INT_EN_IN)
#define HWIO_INTERRUPT_AGG_DONE_INT_EN_EN_BMSK                                     0xffff
#define HWIO_INTERRUPT_AGG_DONE_INT_EN_EN_SHFT                                        0x0

#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_ADDR                              (BCM_INTERRUPT_REG_BASE      + 0x00000004)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_RMSK                                  0xffff
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_IN          \
        in_dword_masked(HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_ADDR, HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_RMSK)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_ADDR, m)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_OUT(v)      \
        out_dword(HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_ADDR,v)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_ADDR,m,v,HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_IN)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_EN_BMSK                               0xffff
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_EN_SHFT                                  0x0

#define HWIO_INTERRUPT_CTT_FULL_INT_EN_ADDR                                    (BCM_INTERRUPT_REG_BASE      + 0x00000008)
#define HWIO_INTERRUPT_CTT_FULL_INT_EN_RMSK                                         0xfff
#define HWIO_INTERRUPT_CTT_FULL_INT_EN_IN          \
        in_dword_masked(HWIO_INTERRUPT_CTT_FULL_INT_EN_ADDR, HWIO_INTERRUPT_CTT_FULL_INT_EN_RMSK)
#define HWIO_INTERRUPT_CTT_FULL_INT_EN_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_CTT_FULL_INT_EN_ADDR, m)
#define HWIO_INTERRUPT_CTT_FULL_INT_EN_OUT(v)      \
        out_dword(HWIO_INTERRUPT_CTT_FULL_INT_EN_ADDR,v)
#define HWIO_INTERRUPT_CTT_FULL_INT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_INTERRUPT_CTT_FULL_INT_EN_ADDR,m,v,HWIO_INTERRUPT_CTT_FULL_INT_EN_IN)
#define HWIO_INTERRUPT_CTT_FULL_INT_EN_EN_BMSK                                      0xfff
#define HWIO_INTERRUPT_CTT_FULL_INT_EN_EN_SHFT                                        0x0

#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_ADDR                               (BCM_INTERRUPT_REG_BASE      + 0x0000000c)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_RMSK                                   0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_IN          \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_ADDR, HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_RMSK)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_ADDR, m)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_OUT(v)      \
        out_dword(HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_ADDR,v)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_ADDR,m,v,HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_IN)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_EN_BMSK                                0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_EN_SHFT                                   0x0

#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_ADDR                               (BCM_INTERRUPT_REG_BASE      + 0x00000010)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_RMSK                                   0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_IN          \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_ADDR, HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_RMSK)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_ADDR, m)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_OUT(v)      \
        out_dword(HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_ADDR,v)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_ADDR,m,v,HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_IN)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_EN_BMSK                                0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_EN_SHFT                                   0x0

#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_ADDR                                (BCM_INTERRUPT_REG_BASE      + 0x00000014)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_RMSK                                    0xffff
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_IN          \
        in_dword_masked(HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_ADDR, HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_RMSK)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_ADDR, m)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_OUT(v)      \
        out_dword(HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_ADDR,v)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_ADDR,m,v,HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_IN)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_EN_BMSK                                 0xffff
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_EN_SHFT                                    0x0

#define HWIO_INTERRUPT_AGG_DONE_INT_STATUS_ADDR                                (BCM_INTERRUPT_REG_BASE      + 0x00000040)
#define HWIO_INTERRUPT_AGG_DONE_INT_STATUS_RMSK                                    0xffff
#define HWIO_INTERRUPT_AGG_DONE_INT_STATUS_IN          \
        in_dword_masked(HWIO_INTERRUPT_AGG_DONE_INT_STATUS_ADDR, HWIO_INTERRUPT_AGG_DONE_INT_STATUS_RMSK)
#define HWIO_INTERRUPT_AGG_DONE_INT_STATUS_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_AGG_DONE_INT_STATUS_ADDR, m)
#define HWIO_INTERRUPT_AGG_DONE_INT_STATUS_STATUS_BMSK                             0xffff
#define HWIO_INTERRUPT_AGG_DONE_INT_STATUS_STATUS_SHFT                                0x0

#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_ADDR                          (BCM_INTERRUPT_REG_BASE      + 0x00000044)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_RMSK                              0xffff
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_IN          \
        in_dword_masked(HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_ADDR, HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_RMSK)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_ADDR, m)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_STATUS_BMSK                       0xffff
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_STATUS_SHFT                          0x0

#define HWIO_INTERRUPT_CTT_FULL_INT_STATUS_ADDR                                (BCM_INTERRUPT_REG_BASE      + 0x00000048)
#define HWIO_INTERRUPT_CTT_FULL_INT_STATUS_RMSK                                     0xfff
#define HWIO_INTERRUPT_CTT_FULL_INT_STATUS_IN          \
        in_dword_masked(HWIO_INTERRUPT_CTT_FULL_INT_STATUS_ADDR, HWIO_INTERRUPT_CTT_FULL_INT_STATUS_RMSK)
#define HWIO_INTERRUPT_CTT_FULL_INT_STATUS_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_CTT_FULL_INT_STATUS_ADDR, m)
#define HWIO_INTERRUPT_CTT_FULL_INT_STATUS_STATUS_BMSK                              0xfff
#define HWIO_INTERRUPT_CTT_FULL_INT_STATUS_STATUS_SHFT                                0x0

#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_ADDR                           (BCM_INTERRUPT_REG_BASE      + 0x0000004c)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_RMSK                               0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_IN          \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_ADDR, HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_RMSK)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_ADDR, m)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_STATUS_BMSK                        0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_STATUS_SHFT                           0x0

#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_ADDR                           (BCM_INTERRUPT_REG_BASE      + 0x00000050)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_RMSK                               0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_IN          \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_ADDR, HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_RMSK)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_ADDR, m)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_STATUS_BMSK                        0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_STATUS_SHFT                           0x0

#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_ADDR                            (BCM_INTERRUPT_REG_BASE      + 0x00000054)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_RMSK                                0xffff
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_IN          \
        in_dword_masked(HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_ADDR, HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_RMSK)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_ADDR, m)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_STATUS_BMSK                         0xffff
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_STATUS_SHFT                            0x0

#define HWIO_INTERRUPT_AGG_DONE_INT_CLEAR_ADDR                                 (BCM_INTERRUPT_REG_BASE      + 0x00000080)
#define HWIO_INTERRUPT_AGG_DONE_INT_CLEAR_RMSK                                     0xffff
#define HWIO_INTERRUPT_AGG_DONE_INT_CLEAR_OUT(v)      \
        out_dword(HWIO_INTERRUPT_AGG_DONE_INT_CLEAR_ADDR,v)
#define HWIO_INTERRUPT_AGG_DONE_INT_CLEAR_CLEAR_BMSK                               0xffff
#define HWIO_INTERRUPT_AGG_DONE_INT_CLEAR_CLEAR_SHFT                                  0x0

#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_CLEAR_ADDR                           (BCM_INTERRUPT_REG_BASE      + 0x00000084)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_CLEAR_RMSK                               0xffff
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_CLEAR_OUT(v)      \
        out_dword(HWIO_INTERRUPT_CP_CHANGE_DONE_INT_CLEAR_ADDR,v)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_CLEAR_CLEAR_BMSK                         0xffff
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_CLEAR_CLEAR_SHFT                            0x0

#define HWIO_INTERRUPT_CTT_FULL_INT_CLEAR_ADDR                                 (BCM_INTERRUPT_REG_BASE      + 0x00000088)
#define HWIO_INTERRUPT_CTT_FULL_INT_CLEAR_RMSK                                      0xfff
#define HWIO_INTERRUPT_CTT_FULL_INT_CLEAR_OUT(v)      \
        out_dword(HWIO_INTERRUPT_CTT_FULL_INT_CLEAR_ADDR,v)
#define HWIO_INTERRUPT_CTT_FULL_INT_CLEAR_CLEAR_BMSK                                0xfff
#define HWIO_INTERRUPT_CTT_FULL_INT_CLEAR_CLEAR_SHFT                                  0x0

#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_CLEAR_ADDR                            (BCM_INTERRUPT_REG_BASE      + 0x0000008c)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_CLEAR_RMSK                                0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_CLEAR_OUT(v)      \
        out_dword(HWIO_INTERRUPT_SEQ_GENERIC_INT_0_CLEAR_ADDR,v)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_CLEAR_CLEAR_BMSK                          0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_CLEAR_CLEAR_SHFT                             0x0

#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_CLEAR_ADDR                            (BCM_INTERRUPT_REG_BASE      + 0x00000090)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_CLEAR_RMSK                                0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_CLEAR_OUT(v)      \
        out_dword(HWIO_INTERRUPT_SEQ_GENERIC_INT_1_CLEAR_ADDR,v)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_CLEAR_CLEAR_BMSK                          0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_CLEAR_CLEAR_SHFT                             0x0

#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_CLEAR_ADDR                             (BCM_INTERRUPT_REG_BASE      + 0x00000094)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_CLEAR_RMSK                                 0xffff
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_CLEAR_OUT(v)      \
        out_dword(HWIO_INTERRUPT_STOP_AGG_ACK_INT_CLEAR_ADDR,v)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_CLEAR_CLEAR_BMSK                           0xffff
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_CLEAR_CLEAR_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: BCM_AU_MEM
 *--------------------------------------------------------------------------*/

#define BCM_AU_MEM_REG_BASE                                  (AOSS_BASE      + 0x00bf2000)

#define HWIO_AU_INST_MEMa_ADDR(a)                            (BCM_AU_MEM_REG_BASE      + 0x00000000 + 0x4 * (a))
#define HWIO_AU_INST_MEMa_RMSK                               0xffffffff
#define HWIO_AU_INST_MEMa_MAXa                                      255
#define HWIO_AU_INST_MEMa_INI(a)        \
        in_dword_masked(HWIO_AU_INST_MEMa_ADDR(a), HWIO_AU_INST_MEMa_RMSK)
#define HWIO_AU_INST_MEMa_INMI(a,mask)    \
        in_dword_masked(HWIO_AU_INST_MEMa_ADDR(a), mask)
#define HWIO_AU_INST_MEMa_OUTI(a,val)    \
        out_dword(HWIO_AU_INST_MEMa_ADDR(a),val)
#define HWIO_AU_INST_MEMa_OUTMI(a,mask,val) \
        out_dword_masked_ns(HWIO_AU_INST_MEMa_ADDR(a),mask,val,HWIO_AU_INST_MEMa_INI(a))
#define HWIO_AU_INST_MEMa_INSTRUCTION_BMSK                   0xffffffff
#define HWIO_AU_INST_MEMa_INSTRUCTION_SHFT                          0x0

/*----------------------------------------------------------------------------
 * MODULE: BCM_SEQ_MEM
 *--------------------------------------------------------------------------*/

#define BCM_SEQ_MEM_REG_BASE                     (AOSS_BASE      + 0x00bf2400)

#define HWIO_SEQ_MEMq_ADDR(q)                    (BCM_SEQ_MEM_REG_BASE      + 0x00000000 + 0x4 * (q))
#define HWIO_SEQ_MEMq_RMSK                       0xffffffff
#define HWIO_SEQ_MEMq_MAXq                               31
#define HWIO_SEQ_MEMq_INI(q)        \
        in_dword_masked(HWIO_SEQ_MEMq_ADDR(q), HWIO_SEQ_MEMq_RMSK)
#define HWIO_SEQ_MEMq_INMI(q,mask)    \
        in_dword_masked(HWIO_SEQ_MEMq_ADDR(q), mask)
#define HWIO_SEQ_MEMq_OUTI(q,val)    \
        out_dword(HWIO_SEQ_MEMq_ADDR(q),val)
#define HWIO_SEQ_MEMq_OUTMI(q,mask,val) \
        out_dword_masked_ns(HWIO_SEQ_MEMq_ADDR(q),mask,val,HWIO_SEQ_MEMq_INI(q))
#define HWIO_SEQ_MEMq_CMD_BMSK                   0xffffffff
#define HWIO_SEQ_MEMq_CMD_SHFT                          0x0

/*----------------------------------------------------------------------------
 * MODULE: BCM_SW_OVERRIDE
 *--------------------------------------------------------------------------*/

#define BCM_SW_OVERRIDE_REG_BASE                                                            (AOSS_BASE      + 0x00bf2600)

#define HWIO_SW_CP_CDm_ADDR(m)                                                              (BCM_SW_OVERRIDE_REG_BASE      + 0x00000000 + 0x4 * (m))
#define HWIO_SW_CP_CDm_RMSK                                                                       0x1f
#define HWIO_SW_CP_CDm_MAXm                                                                         15
#define HWIO_SW_CP_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_CP_CDm_ADDR(m), HWIO_SW_CP_CDm_RMSK)
#define HWIO_SW_CP_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_CP_CDm_ADDR(m), mask)
#define HWIO_SW_CP_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_CP_CDm_ADDR(m),val)
#define HWIO_SW_CP_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_CP_CDm_ADDR(m),mask,val,HWIO_SW_CP_CDm_INI(m))
#define HWIO_SW_CP_CDm_VALID_BMSK                                                                 0x10
#define HWIO_SW_CP_CDm_VALID_SHFT                                                                  0x4
#define HWIO_SW_CP_CDm_SW_CP_BMSK                                                                  0xf
#define HWIO_SW_CP_CDm_SW_CP_SHFT                                                                  0x0

#define HWIO_SW_MASK_FE_BE_TRIG_CDm_ADDR(m)                                                 (BCM_SW_OVERRIDE_REG_BASE      + 0x00000080 + 0x4 * (m))
#define HWIO_SW_MASK_FE_BE_TRIG_CDm_RMSK                                                           0x1
#define HWIO_SW_MASK_FE_BE_TRIG_CDm_MAXm                                                            15
#define HWIO_SW_MASK_FE_BE_TRIG_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_MASK_FE_BE_TRIG_CDm_ADDR(m), HWIO_SW_MASK_FE_BE_TRIG_CDm_RMSK)
#define HWIO_SW_MASK_FE_BE_TRIG_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_MASK_FE_BE_TRIG_CDm_ADDR(m), mask)
#define HWIO_SW_MASK_FE_BE_TRIG_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_MASK_FE_BE_TRIG_CDm_ADDR(m),val)
#define HWIO_SW_MASK_FE_BE_TRIG_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_MASK_FE_BE_TRIG_CDm_ADDR(m),mask,val,HWIO_SW_MASK_FE_BE_TRIG_CDm_INI(m))
#define HWIO_SW_MASK_FE_BE_TRIG_CDm_MASK_BMSK                                                      0x1
#define HWIO_SW_MASK_FE_BE_TRIG_CDm_MASK_SHFT                                                      0x0

#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_ADDR(m)                                               (BCM_SW_OVERRIDE_REG_BASE      + 0x00000100 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_RMSK                                                         0x1
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_MAXm                                                          15
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERRIDE_STOP_AGG_CDm_ADDR(m), HWIO_SW_OVERRIDE_STOP_AGG_CDm_RMSK)
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERRIDE_STOP_AGG_CDm_ADDR(m), mask)
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_STOP_AGG_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERRIDE_STOP_AGG_CDm_ADDR(m),mask,val,HWIO_SW_OVERRIDE_STOP_AGG_CDm_INI(m))
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_SW_STOP_AGG_BMSK                                             0x1
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_SW_STOP_AGG_SHFT                                             0x0

#define HWIO_SW_PERMIT_ONE_AGG_CDm_ADDR(m)                                                  (BCM_SW_OVERRIDE_REG_BASE      + 0x00000180 + 0x4 * (m))
#define HWIO_SW_PERMIT_ONE_AGG_CDm_RMSK                                                            0x1
#define HWIO_SW_PERMIT_ONE_AGG_CDm_MAXm                                                             15
#define HWIO_SW_PERMIT_ONE_AGG_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_PERMIT_ONE_AGG_CDm_ADDR(m),val)
#define HWIO_SW_PERMIT_ONE_AGG_CDm_PERMIT_BMSK                                                     0x1
#define HWIO_SW_PERMIT_ONE_AGG_CDm_PERMIT_SHFT                                                     0x0

#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_ADDR(m)                                              (BCM_SW_OVERRIDE_REG_BASE      + 0x00000200 + 0x4 * (m))
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_RMSK                                                        0xf
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_MAXm                                                         15
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERWRITE_FINAL_CP_CDm_ADDR(m), HWIO_SW_OVERWRITE_FINAL_CP_CDm_RMSK)
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERWRITE_FINAL_CP_CDm_ADDR(m), mask)
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERWRITE_FINAL_CP_CDm_ADDR(m),val)
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERWRITE_FINAL_CP_CDm_ADDR(m),mask,val,HWIO_SW_OVERWRITE_FINAL_CP_CDm_INI(m))
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_FINAL_CP_BMSK                                               0xf
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_FINAL_CP_SHFT                                               0x0

#define HWIO_SW_BLOCK_FINAL_CP_CDm_ADDR(m)                                                  (BCM_SW_OVERRIDE_REG_BASE      + 0x00000280 + 0x4 * (m))
#define HWIO_SW_BLOCK_FINAL_CP_CDm_RMSK                                                            0x1
#define HWIO_SW_BLOCK_FINAL_CP_CDm_MAXm                                                             15
#define HWIO_SW_BLOCK_FINAL_CP_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_BLOCK_FINAL_CP_CDm_ADDR(m), HWIO_SW_BLOCK_FINAL_CP_CDm_RMSK)
#define HWIO_SW_BLOCK_FINAL_CP_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_BLOCK_FINAL_CP_CDm_ADDR(m), mask)
#define HWIO_SW_BLOCK_FINAL_CP_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_BLOCK_FINAL_CP_CDm_ADDR(m),val)
#define HWIO_SW_BLOCK_FINAL_CP_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_BLOCK_FINAL_CP_CDm_ADDR(m),mask,val,HWIO_SW_BLOCK_FINAL_CP_CDm_INI(m))
#define HWIO_SW_BLOCK_FINAL_CP_CDm_BLOCK_BMSK                                                      0x1
#define HWIO_SW_BLOCK_FINAL_CP_CDm_BLOCK_SHFT                                                      0x0

#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_ADDR(m)                                            (BCM_SW_OVERRIDE_REG_BASE      + 0x00000300 + 0x4 * (m))
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_RMSK                                                      0xf
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_MAXm                                                       15
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_ADDR(m), HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_RMSK)
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_ADDR(m), mask)
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_ADDR(m),val)
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_ADDR(m),mask,val,HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_INI(m))
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_WRITTEN_CP_BMSK                                           0xf
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_WRITTEN_CP_SHFT                                           0x0

#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_ADDR(m)                                            (BCM_SW_OVERRIDE_REG_BASE      + 0x00000380 + 0x4 * (m))
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_RMSK                                                      0xf
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_MAXm                                                       15
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERWRITE_CURRENT_CP_CDm_ADDR(m), HWIO_SW_OVERWRITE_CURRENT_CP_CDm_RMSK)
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERWRITE_CURRENT_CP_CDm_ADDR(m), mask)
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERWRITE_CURRENT_CP_CDm_ADDR(m),val)
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERWRITE_CURRENT_CP_CDm_ADDR(m),mask,val,HWIO_SW_OVERWRITE_CURRENT_CP_CDm_INI(m))
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_CURRENT_CP_BMSK                                           0xf
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_CURRENT_CP_SHFT                                           0x0

#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_ADDR(m)                                             (BCM_SW_OVERRIDE_REG_BASE      + 0x00000400 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_RMSK                                                       0x7
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_MAXm                                                        15
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_ADDR(m), HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_RMSK)
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_ADDR(m), mask)
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_ADDR(m),mask,val,HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_INI(m))
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_CP_SEL_BMSK                                                0x7
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_CP_SEL_SHFT                                                0x0

#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_ADDR(m)                                             (BCM_SW_OVERRIDE_REG_BASE      + 0x00000480 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_RMSK                                                       0x1
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_MAXm                                                        15
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_ADDR(m), HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_RMSK)
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_ADDR(m), mask)
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_ADDR(m),mask,val,HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_INI(m))
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_ENABLE_BMSK                                                0x1
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_ENABLE_SHFT                                                0x0

#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_ADDR(m)                                              (BCM_SW_OVERRIDE_REG_BASE      + 0x00000500 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_RMSK                                                        0x1
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_MAXm                                                         15
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_ADDR(m), HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_RMSK)
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_ADDR(m), mask)
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_ADDR(m),mask,val,HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_INI(m))
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_FLUSH_BMSK                                                  0x1
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_FLUSH_SHFT                                                  0x0

#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_ADDR(m)                                     (BCM_SW_OVERRIDE_REG_BASE      + 0x00000580 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_RMSK                                              0x11
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_MAXm                                                15
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_ADDR(m), HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_RMSK)
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_ADDR(m), mask)
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_ADDR(m),mask,val,HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_INI(m))
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_SEQ_UNBLOCK_STICKY_B_BMSK                         0x10
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_SEQ_UNBLOCK_STICKY_B_SHFT                          0x4
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_SEQ_UNBLOCK_STICKY_A_BMSK                          0x1
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_SEQ_UNBLOCK_STICKY_A_SHFT                          0x0

#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm_ADDR(m)                                     (BCM_SW_OVERRIDE_REG_BASE      + 0x00000600 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm_RMSK                                               0x1
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm_MAXm                                                15
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm_SEQ_UNBLOCK_ONCE_A_BMSK                            0x1
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm_SEQ_UNBLOCK_ONCE_A_SHFT                            0x0

#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm_ADDR(m)                                     (BCM_SW_OVERRIDE_REG_BASE      + 0x00000680 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm_RMSK                                               0x1
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm_MAXm                                                15
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm_SEQ_UNBLOCK_ONCE_B_BMSK                            0x1
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm_SEQ_UNBLOCK_ONCE_B_SHFT                            0x0

#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_ADDR(m)                                               (BCM_SW_OVERRIDE_REG_BASE      + 0x00000700 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_RMSK                                                        0x11
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_MAXm                                                          15
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERRIDE_CE_INPUT_CDm_ADDR(m), HWIO_SW_OVERRIDE_CE_INPUT_CDm_RMSK)
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERRIDE_CE_INPUT_CDm_ADDR(m), mask)
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_CE_INPUT_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERRIDE_CE_INPUT_CDm_ADDR(m),mask,val,HWIO_SW_OVERRIDE_CE_INPUT_CDm_INI(m))
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_VALID_BMSK                                                  0x10
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_VALID_SHFT                                                   0x4
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_AGG_DONE_BMSK                                                0x1
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_AGG_DONE_SHFT                                                0x0

#define HWIO_SW_OVERRIDE_BE_SEQ_TCS_CLK_ON_CDm_ADDR(m)                                      (BCM_SW_OVERRIDE_REG_BASE      + 0x00000780 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_BE_SEQ_TCS_CLK_ON_CDm_RMSK                                                0x1
#define HWIO_SW_OVERRIDE_BE_SEQ_TCS_CLK_ON_CDm_MAXm                                                 15
#define HWIO_SW_OVERRIDE_BE_SEQ_TCS_CLK_ON_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERRIDE_BE_SEQ_TCS_CLK_ON_CDm_ADDR(m), HWIO_SW_OVERRIDE_BE_SEQ_TCS_CLK_ON_CDm_RMSK)
#define HWIO_SW_OVERRIDE_BE_SEQ_TCS_CLK_ON_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERRIDE_BE_SEQ_TCS_CLK_ON_CDm_ADDR(m), mask)
#define HWIO_SW_OVERRIDE_BE_SEQ_TCS_CLK_ON_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_BE_SEQ_TCS_CLK_ON_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_BE_SEQ_TCS_CLK_ON_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERRIDE_BE_SEQ_TCS_CLK_ON_CDm_ADDR(m),mask,val,HWIO_SW_OVERRIDE_BE_SEQ_TCS_CLK_ON_CDm_INI(m))
#define HWIO_SW_OVERRIDE_BE_SEQ_TCS_CLK_ON_CDm_FORCE_CLK_EN_BMSK                                   0x1
#define HWIO_SW_OVERRIDE_BE_SEQ_TCS_CLK_ON_CDm_FORCE_CLK_EN_SHFT                                   0x0


#endif /* __BCM_HWIO_H__ */
