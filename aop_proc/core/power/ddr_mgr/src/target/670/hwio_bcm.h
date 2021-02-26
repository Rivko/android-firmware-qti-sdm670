#ifndef __HWIO_BCM_H__
#define __HWIO_BCM_H__
/*
===========================================================================
*/
/**
  @file hwio_bcm.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r44.0.0_MTO]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_BCM_BCM_STATUS

  'Include' filters applied: 
  'Exclude' filters applied: 
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/ddr_mgr/src/target/670/hwio_bcm.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: BCM_STATUS
 *--------------------------------------------------------------------------*/

#define BCM_STATUS_REG_BASE                                                     (AOSS_BASE      + 0x00bf0000)
#define BCM_STATUS_REG_BASE_OFFS                                                0x00bf0000

#define HWIO_STATUS_FE_AGG_FSM_ADDR                                             (BCM_STATUS_REG_BASE      + 0x00000000)
#define HWIO_STATUS_FE_AGG_FSM_OFFS                                             (BCM_STATUS_REG_BASE_OFFS + 0x00000000)
#define HWIO_STATUS_FE_AGG_FSM_RMSK                                                 0xffff
#define HWIO_STATUS_FE_AGG_FSM_IN          \
        in_dword_masked(HWIO_STATUS_FE_AGG_FSM_ADDR, HWIO_STATUS_FE_AGG_FSM_RMSK)
#define HWIO_STATUS_FE_AGG_FSM_INM(m)      \
        in_dword_masked(HWIO_STATUS_FE_AGG_FSM_ADDR, m)
#define HWIO_STATUS_FE_AGG_FSM_AGG_BUSY_CD_BMSK                                     0xffff
#define HWIO_STATUS_FE_AGG_FSM_AGG_BUSY_CD_SHFT                                        0x0

#define HWIO_STATUS_FE_CDm_ADDR(m)                                              (BCM_STATUS_REG_BASE      + 0x00000004 + 0x4 * (m))
#define HWIO_STATUS_FE_CDm_OFFS(m)                                              (BCM_STATUS_REG_BASE_OFFS + 0x00000004 + 0x4 * (m))
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
#define HWIO_STATUS_DIRTY_ND_0_OFFS                                             (BCM_STATUS_REG_BASE_OFFS + 0x00000084)
#define HWIO_STATUS_DIRTY_ND_0_RMSK                                             0xffffffff
#define HWIO_STATUS_DIRTY_ND_0_IN          \
        in_dword_masked(HWIO_STATUS_DIRTY_ND_0_ADDR, HWIO_STATUS_DIRTY_ND_0_RMSK)
#define HWIO_STATUS_DIRTY_ND_0_INM(m)      \
        in_dword_masked(HWIO_STATUS_DIRTY_ND_0_ADDR, m)
#define HWIO_STATUS_DIRTY_ND_0_DIRTY_ND_31_0_BMSK                               0xffffffff
#define HWIO_STATUS_DIRTY_ND_0_DIRTY_ND_31_0_SHFT                                      0x0

#define HWIO_STATUS_DIRTY_ND_1_ADDR                                             (BCM_STATUS_REG_BASE      + 0x00000088)
#define HWIO_STATUS_DIRTY_ND_1_OFFS                                             (BCM_STATUS_REG_BASE_OFFS + 0x00000088)
#define HWIO_STATUS_DIRTY_ND_1_RMSK                                             0xffffffff
#define HWIO_STATUS_DIRTY_ND_1_IN          \
        in_dword_masked(HWIO_STATUS_DIRTY_ND_1_ADDR, HWIO_STATUS_DIRTY_ND_1_RMSK)
#define HWIO_STATUS_DIRTY_ND_1_INM(m)      \
        in_dword_masked(HWIO_STATUS_DIRTY_ND_1_ADDR, m)
#define HWIO_STATUS_DIRTY_ND_1_DIRTY_ND_63_32_BMSK                              0xffffffff
#define HWIO_STATUS_DIRTY_ND_1_DIRTY_ND_63_32_SHFT                                     0x0

#define HWIO_STATUS_SNDs_ADDR(s)                                                (BCM_STATUS_REG_BASE      + 0x0000008c + 0x4 * (s))
#define HWIO_STATUS_SNDs_OFFS(s)                                                (BCM_STATUS_REG_BASE_OFFS + 0x0000008c + 0x4 * (s))
#define HWIO_STATUS_SNDs_RMSK                                                          0xf
#define HWIO_STATUS_SNDs_MAXs                                                            7
#define HWIO_STATUS_SNDs_INI(s)        \
        in_dword_masked(HWIO_STATUS_SNDs_ADDR(s), HWIO_STATUS_SNDs_RMSK)
#define HWIO_STATUS_SNDs_INMI(s,mask)    \
        in_dword_masked(HWIO_STATUS_SNDs_ADDR(s), mask)
#define HWIO_STATUS_SNDs_SEL_CP_BMSK                                                   0xf
#define HWIO_STATUS_SNDs_SEL_CP_SHFT                                                   0x0

#define HWIO_STATUS_AGG_BW_NDd_ADDR(d)                                          (BCM_STATUS_REG_BASE      + 0x0000010c + 0x4 * (d))
#define HWIO_STATUS_AGG_BW_NDd_OFFS(d)                                          (BCM_STATUS_REG_BASE_OFFS + 0x0000010c + 0x4 * (d))
#define HWIO_STATUS_AGG_BW_NDd_RMSK                                                 0x7fff
#define HWIO_STATUS_AGG_BW_NDd_MAXd                                                     63
#define HWIO_STATUS_AGG_BW_NDd_INI(d)        \
        in_dword_masked(HWIO_STATUS_AGG_BW_NDd_ADDR(d), HWIO_STATUS_AGG_BW_NDd_RMSK)
#define HWIO_STATUS_AGG_BW_NDd_INMI(d,mask)    \
        in_dword_masked(HWIO_STATUS_AGG_BW_NDd_ADDR(d), mask)
#define HWIO_STATUS_AGG_BW_NDd_AGG_BW_BMSK                                          0x7fff
#define HWIO_STATUS_AGG_BW_NDd_AGG_BW_SHFT                                             0x0

#define HWIO_STATUS_AGG_BW_Z_NDs_ADDR(s)                                        (BCM_STATUS_REG_BASE      + 0x0000030c + 0x4 * (s))
#define HWIO_STATUS_AGG_BW_Z_NDs_OFFS(s)                                        (BCM_STATUS_REG_BASE_OFFS + 0x0000030c + 0x4 * (s))
#define HWIO_STATUS_AGG_BW_Z_NDs_RMSK                                               0x7fff
#define HWIO_STATUS_AGG_BW_Z_NDs_MAXs                                                    7
#define HWIO_STATUS_AGG_BW_Z_NDs_INI(s)        \
        in_dword_masked(HWIO_STATUS_AGG_BW_Z_NDs_ADDR(s), HWIO_STATUS_AGG_BW_Z_NDs_RMSK)
#define HWIO_STATUS_AGG_BW_Z_NDs_INMI(s,mask)    \
        in_dword_masked(HWIO_STATUS_AGG_BW_Z_NDs_ADDR(s), mask)
#define HWIO_STATUS_AGG_BW_Z_NDs_AGG_BW_Z_BMSK                                      0x7fff
#define HWIO_STATUS_AGG_BW_Z_NDs_AGG_BW_Z_SHFT                                         0x0

#define HWIO_STATUS_BE_CDm_ADDR(m)                                              (BCM_STATUS_REG_BASE      + 0x00000800 + 0x4 * (m))
#define HWIO_STATUS_BE_CDm_OFFS(m)                                              (BCM_STATUS_REG_BASE_OFFS + 0x00000800 + 0x4 * (m))
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
#define HWIO_STATUS_BE_SEQ_CDm_OFFS(m)                                          (BCM_STATUS_REG_BASE_OFFS + 0x00000880 + 0x4 * (m))
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
#define HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_OFFS(m)                             (BCM_STATUS_REG_BASE_OFFS + 0x00000900 + 0x4 * (m))
#define HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_RMSK                                       0xf
#define HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_MAXm                                        15
#define HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_INI(m)        \
        in_dword_masked(HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_ADDR(m), HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_RMSK)
#define HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_ADDR(m), mask)
#define HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_ISSUE_STATUS_BMSK                          0xf
#define HWIO_STATUS_BE_CMD_ISSUE_STATUS_CDm_ISSUE_STATUS_SHFT                          0x0

#define HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_ADDR(m)                              (BCM_STATUS_REG_BASE      + 0x00000980 + 0x4 * (m))
#define HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_OFFS(m)                              (BCM_STATUS_REG_BASE_OFFS + 0x00000980 + 0x4 * (m))
#define HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_RMSK                                        0xf
#define HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_MAXm                                         15
#define HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_INI(m)        \
        in_dword_masked(HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_ADDR(m), HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_RMSK)
#define HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_ADDR(m), mask)
#define HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_SENT_STATUS_BMSK                            0xf
#define HWIO_STATUS_BE_CMD_SENT_STATUS_CDm_SENT_STATUS_SHFT                            0x0

#define HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_ADDR(m)                              (BCM_STATUS_REG_BASE      + 0x00000a00 + 0x4 * (m))
#define HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_OFFS(m)                              (BCM_STATUS_REG_BASE_OFFS + 0x00000a00 + 0x4 * (m))
#define HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_RMSK                                        0xf
#define HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_MAXm                                         15
#define HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_INI(m)        \
        in_dword_masked(HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_ADDR(m), HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_RMSK)
#define HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_ADDR(m), mask)
#define HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_CMPL_STATUS_BMSK                            0xf
#define HWIO_STATUS_BE_CMD_CMPL_STATUS_CDm_CMPL_STATUS_SHFT                            0x0

#define HWIO_STATUS_BE_TCS_STATUS_CDm_ADDR(m)                                   (BCM_STATUS_REG_BASE      + 0x00000a80 + 0x4 * (m))
#define HWIO_STATUS_BE_TCS_STATUS_CDm_OFFS(m)                                   (BCM_STATUS_REG_BASE_OFFS + 0x00000a80 + 0x4 * (m))
#define HWIO_STATUS_BE_TCS_STATUS_CDm_RMSK                                             0x7
#define HWIO_STATUS_BE_TCS_STATUS_CDm_MAXm                                              15
#define HWIO_STATUS_BE_TCS_STATUS_CDm_INI(m)        \
        in_dword_masked(HWIO_STATUS_BE_TCS_STATUS_CDm_ADDR(m), HWIO_STATUS_BE_TCS_STATUS_CDm_RMSK)
#define HWIO_STATUS_BE_TCS_STATUS_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_STATUS_BE_TCS_STATUS_CDm_ADDR(m), mask)
#define HWIO_STATUS_BE_TCS_STATUS_CDm_TCS_CURR_STATE_BMSK                              0x7
#define HWIO_STATUS_BE_TCS_STATUS_CDm_TCS_CURR_STATE_SHFT                              0x0

#define HWIO_STATUS_CE_DRVr_CTTe_ADDR(r,e)                                      (BCM_STATUS_REG_BASE      + 0x00000f00 + 0x60 * (r) + 0x4 * (e))
#define HWIO_STATUS_CE_DRVr_CTTe_OFFS(r,e)                                      (BCM_STATUS_REG_BASE_OFFS + 0x00000f00 + 0x60 * (r) + 0x4 * (e))
#define HWIO_STATUS_CE_DRVr_CTTe_RMSK                                           0xffff07ff
#define HWIO_STATUS_CE_DRVr_CTTe_MAXr                                                   12
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
#define HWIO_STATUS_CE_OFFS                                                     (BCM_STATUS_REG_BASE_OFFS + 0x00001700)
#define HWIO_STATUS_CE_RMSK                                                       0x1fff01
#define HWIO_STATUS_CE_IN          \
        in_dword_masked(HWIO_STATUS_CE_ADDR, HWIO_STATUS_CE_RMSK)
#define HWIO_STATUS_CE_INM(m)      \
        in_dword_masked(HWIO_STATUS_CE_ADDR, m)
#define HWIO_STATUS_CE_CTT_EMPTY_DRV_BMSK                                         0x1fff00
#define HWIO_STATUS_CE_CTT_EMPTY_DRV_SHFT                                              0x8
#define HWIO_STATUS_CE_CTT_EMPTY_BMSK                                                  0x1
#define HWIO_STATUS_CE_CTT_EMPTY_SHFT                                                  0x0

#define HWIO_STATUS_BCM_ADDR                                                    (BCM_STATUS_REG_BASE      + 0x00001704)
#define HWIO_STATUS_BCM_OFFS                                                    (BCM_STATUS_REG_BASE_OFFS + 0x00001704)
#define HWIO_STATUS_BCM_RMSK                                                           0x1
#define HWIO_STATUS_BCM_IN          \
        in_dword_masked(HWIO_STATUS_BCM_ADDR, HWIO_STATUS_BCM_RMSK)
#define HWIO_STATUS_BCM_INM(m)      \
        in_dword_masked(HWIO_STATUS_BCM_ADDR, m)
#define HWIO_STATUS_BCM_BCM_IDLE_BMSK                                                  0x1
#define HWIO_STATUS_BCM_BCM_IDLE_SHFT                                                  0x0

#define HWIO_HW_VERSION_ADDR                                                    (BCM_STATUS_REG_BASE      + 0x00001708)
#define HWIO_HW_VERSION_OFFS                                                    (BCM_STATUS_REG_BASE_OFFS + 0x00001708)
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


#endif /* __HWIO_BCM_H__ */
