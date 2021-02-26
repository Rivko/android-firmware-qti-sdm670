#ifndef __HWSOLVER_HWIO_H__
#define __HWSOLVER_HWIO_H__
/*
===========================================================================
*/
/**
@file hwsolver_hwio.h
  @brief Auto-generated HWIO interface include file.

Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p2q1r89]
 
This file contains HWIO register definitions for the following modules:
  SSC_RSCC_RSCC_RSC
    SSC_QTMR_AC
    SSC_QTMR_F0_0

'Include' filters applied: 
  'Exclude' filters applied: DUMMY RESERVED 
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

  $Header: //components/rel/core.qdsp6/2.1/power/hwsolver/src/hwsolver_hwio.h#2 $
  $DateTime: 2017/08/08 02:18:47 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "HALhwio.h"
#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: SSC_RSCC_RSCC_RSC
 *--------------------------------------------------------------------------*/

#define SSC_RSCC_RSCC_RSC_REG_BASE                                                                              (LPASS_BASE      + 0x00b90000)

#define HWIO_SSC_RSCC_RSC_ID_DRVd_ADDR(d)                                                                       (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000000 + 0x10000 * (d))
#define HWIO_SSC_RSCC_RSC_ID_DRVd_RMSK                                                                            0xffffff
#define HWIO_SSC_RSCC_RSC_ID_DRVd_MAXd                                                                                   0
#define HWIO_SSC_RSCC_RSC_ID_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_ID_DRVd_ADDR(d), HWIO_SSC_RSCC_RSC_ID_DRVd_RMSK)
#define HWIO_SSC_RSCC_RSC_ID_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_ID_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_RSC_ID_DRVd_MAJOR_VER_BMSK                                                                  0xff0000
#define HWIO_SSC_RSCC_RSC_ID_DRVd_MAJOR_VER_SHFT                                                                      0x10
#define HWIO_SSC_RSCC_RSC_ID_DRVd_MINOR_VER_BMSK                                                                    0xff00
#define HWIO_SSC_RSCC_RSC_ID_DRVd_MINOR_VER_SHFT                                                                       0x8
#define HWIO_SSC_RSCC_RSC_ID_DRVd_STEP_VER_BMSK                                                                       0xff
#define HWIO_SSC_RSCC_RSC_ID_DRVd_STEP_VER_SHFT                                                                        0x0

#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(d)                                                      (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000004 + 0x10000 * (d))
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_RMSK                                                          0x71f1f1f
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_MAXd                                                                  0
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(d), HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_RMSK)
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SEQ_EXTENDED_BMSK                                             0x4000000
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SEQ_EXTENDED_SHFT                                                  0x1a
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_IS_CHILD_RSC_BMSK                                             0x2000000
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_IS_CHILD_RSC_SHFT                                                  0x19
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_HW_SOLVER_BMSK                                                0x1000000
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_HW_SOLVER_SHFT                                                     0x18
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_SLOTS_BMSK                                              0x1f0000
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_SLOTS_SHFT                                                  0x10
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_MODES_BMSK                                                0x1f00
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_MODES_SHFT                                                   0x8
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_NUM_TIMERS_BMSK                                                    0x1f
#define HWIO_SSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_NUM_TIMERS_SHFT                                                     0x0

#define HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(d)                                                         (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000008 + 0x10000 * (d))
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_RMSK                                                             0x7ff0f1f
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_MAXd                                                                     0
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(d), HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_RMSK)
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_DRV_BMSK                                                     0x7000000
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_DRV_SHFT                                                          0x18
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_SEQ_CMD_WORDS_BMSK                                            0xff0000
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_SEQ_CMD_WORDS_SHFT                                                0x10
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_TS_EVENTS_BMSK                                                   0xf00
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_TS_EVENTS_SHFT                                                     0x8
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_DELAY_CNTR_BITWIDTH_BMSK                                              0x1f
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_DELAY_CNTR_BITWIDTH_SHFT                                               0x0

#define HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(d)                                             (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x0000000c + 0x10000 * (d))
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_RMSK                                                0xf8ffffff
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_MAXd                                                         0
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(d), HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_RMSK)
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_CMDS_PER_TCS_BMSK                               0xf8000000
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_CMDS_PER_TCS_SHFT                                     0x1b
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV3_BMSK                                     0xfc0000
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV3_SHFT                                         0x12
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV2_BMSK                                      0x3f000
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV2_SHFT                                          0xc
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV1_BMSK                                        0xfc0
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV1_SHFT                                          0x6
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV0_BMSK                                         0x3f
#define HWIO_SSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV0_SHFT                                          0x0

#define HWIO_SSC_RSCC_RSC_STATUS0_DRVd_ADDR(d)                                                                  (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000010 + 0x10000 * (d))
#define HWIO_SSC_RSCC_RSC_STATUS0_DRVd_RMSK                                                                      0x101ffff
#define HWIO_SSC_RSCC_RSC_STATUS0_DRVd_MAXd                                                                              0
#define HWIO_SSC_RSCC_RSC_STATUS0_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_STATUS0_DRVd_ADDR(d), HWIO_SSC_RSCC_RSC_STATUS0_DRVd_RMSK)
#define HWIO_SSC_RSCC_RSC_STATUS0_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_STATUS0_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_RSC_STATUS0_DRVd_OUTI(d,val)    \
        out_dword(HWIO_SSC_RSCC_RSC_STATUS0_DRVd_ADDR(d),val)
#define HWIO_SSC_RSCC_RSC_STATUS0_DRVd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_STATUS0_DRVd_ADDR(d),mask,val,HWIO_SSC_RSCC_RSC_STATUS0_DRVd_INI(d))
#define HWIO_SSC_RSCC_RSC_STATUS0_DRVd_CLK_GATE_DISABLE_BMSK                                                     0x1000000
#define HWIO_SSC_RSCC_RSC_STATUS0_DRVd_CLK_GATE_DISABLE_SHFT                                                          0x18
#define HWIO_SSC_RSCC_RSC_STATUS0_DRVd_STATUS_SLEEP_ENTRY_DONE_BMSK                                                0x10000
#define HWIO_SSC_RSCC_RSC_STATUS0_DRVd_STATUS_SLEEP_ENTRY_DONE_SHFT                                                   0x10
#define HWIO_SSC_RSCC_RSC_STATUS0_DRVd_STATUS_SLEEP_CTL_FSM_BMSK                                                    0xffff
#define HWIO_SSC_RSCC_RSC_STATUS0_DRVd_STATUS_SLEEP_CTL_FSM_SHFT                                                       0x0

#define HWIO_SSC_RSCC_RSC_STATUS1_DRVd_ADDR(d)                                                                  (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000014 + 0x10000 * (d))
#define HWIO_SSC_RSCC_RSC_STATUS1_DRVd_RMSK                                                                     0xffffffff
#define HWIO_SSC_RSCC_RSC_STATUS1_DRVd_MAXd                                                                              0
#define HWIO_SSC_RSCC_RSC_STATUS1_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_STATUS1_DRVd_ADDR(d), HWIO_SSC_RSCC_RSC_STATUS1_DRVd_RMSK)
#define HWIO_SSC_RSCC_RSC_STATUS1_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_STATUS1_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_RSC_STATUS1_DRVd_PDC_MATCH_VALUE_LO_BMSK                                                  0xffffffff
#define HWIO_SSC_RSCC_RSC_STATUS1_DRVd_PDC_MATCH_VALUE_LO_SHFT                                                         0x0

#define HWIO_SSC_RSCC_RSC_STATUS2_DRVd_ADDR(d)                                                                  (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000018 + 0x10000 * (d))
#define HWIO_SSC_RSCC_RSC_STATUS2_DRVd_RMSK                                                                       0xffffff
#define HWIO_SSC_RSCC_RSC_STATUS2_DRVd_MAXd                                                                              0
#define HWIO_SSC_RSCC_RSC_STATUS2_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_STATUS2_DRVd_ADDR(d), HWIO_SSC_RSCC_RSC_STATUS2_DRVd_RMSK)
#define HWIO_SSC_RSCC_RSC_STATUS2_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_STATUS2_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_RSC_STATUS2_DRVd_PDC_MATCH_VALUE_HI_BMSK                                                    0xffffff
#define HWIO_SSC_RSCC_RSC_STATUS2_DRVd_PDC_MATCH_VALUE_HI_SHFT                                                         0x0

#define HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_ADDR                                                                 (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x0000001c)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_RMSK                                                                    0x10003
#define HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_ADDR, HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_RMSK)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_ADDR,m,v,HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_IN)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_TRIGGER_BMSK                                                 0x10000
#define HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_TRIGGER_SHFT                                                    0x10
#define HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_PDC_TIMER_DRV_OWNER_BMSK                                         0x3
#define HWIO_SSC_RSCC_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_PDC_TIMER_DRV_OWNER_SHFT                                         0x0

#define HWIO_SSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR                                                   (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000020)
#define HWIO_SSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_RMSK                                                       0xffff
#define HWIO_SSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR, HWIO_SSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_RMSK)
#define HWIO_SSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR,m,v,HWIO_SSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_IN)
#define HWIO_SSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_PDC_SEQ_START_ADDR_REG_OFFSET_BMSK                         0xffff
#define HWIO_SSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_PDC_SEQ_START_ADDR_REG_OFFSET_SHFT                            0x0

#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR                                                   (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000024)
#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_RMSK                                                       0xffff
#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR, HWIO_SSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_RMSK)
#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR,m,v,HWIO_SSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_IN)
#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_PDC_MATCH_VALUE_LO_REG_OFFSET_BMSK                         0xffff
#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_PDC_MATCH_VALUE_LO_REG_OFFSET_SHFT                            0x0

#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR                                                   (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000028)
#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_RMSK                                                       0xffff
#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR, HWIO_SSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_RMSK)
#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR,m,v,HWIO_SSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_IN)
#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_PDC_MATCH_VALUE_HI_REG_OFFSET_BMSK                         0xffff
#define HWIO_SSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_PDC_MATCH_VALUE_HI_REG_OFFSET_SHFT                            0x0

#define HWIO_SSC_RSCC_PDC_SLAVE_ID_DRV0_ADDR                                                                    (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x0000002c)
#define HWIO_SSC_RSCC_PDC_SLAVE_ID_DRV0_RMSK                                                                           0x7
#define HWIO_SSC_RSCC_PDC_SLAVE_ID_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_PDC_SLAVE_ID_DRV0_ADDR, HWIO_SSC_RSCC_PDC_SLAVE_ID_DRV0_RMSK)
#define HWIO_SSC_RSCC_PDC_SLAVE_ID_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_PDC_SLAVE_ID_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_PDC_SLAVE_ID_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_PDC_SLAVE_ID_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_PDC_SLAVE_ID_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_PDC_SLAVE_ID_DRV0_ADDR,m,v,HWIO_SSC_RSCC_PDC_SLAVE_ID_DRV0_IN)
#define HWIO_SSC_RSCC_PDC_SLAVE_ID_DRV0_PDC_SLAVE_ID_BMSK                                                              0x7
#define HWIO_SSC_RSCC_PDC_SLAVE_ID_DRV0_PDC_SLAVE_ID_SHFT                                                              0x0

#define HWIO_SSC_RSCC_HIDDEN_TCS_STATUS_DRVd_ADDR(d)                                                            (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000030 + 0x10000 * (d))
#define HWIO_SSC_RSCC_HIDDEN_TCS_STATUS_DRVd_RMSK                                                                  0x70707
#define HWIO_SSC_RSCC_HIDDEN_TCS_STATUS_DRVd_MAXd                                                                        0
#define HWIO_SSC_RSCC_HIDDEN_TCS_STATUS_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_STATUS_DRVd_ADDR(d), HWIO_SSC_RSCC_HIDDEN_TCS_STATUS_DRVd_RMSK)
#define HWIO_SSC_RSCC_HIDDEN_TCS_STATUS_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_STATUS_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_HIDDEN_TCS_STATUS_DRVd_COMPLETED_BMSK                                                        0x70000
#define HWIO_SSC_RSCC_HIDDEN_TCS_STATUS_DRVd_COMPLETED_SHFT                                                           0x10
#define HWIO_SSC_RSCC_HIDDEN_TCS_STATUS_DRVd_ISSUED_BMSK                                                             0x700
#define HWIO_SSC_RSCC_HIDDEN_TCS_STATUS_DRVd_ISSUED_SHFT                                                               0x8
#define HWIO_SSC_RSCC_HIDDEN_TCS_STATUS_DRVd_TRIGGERED_BMSK                                                            0x7
#define HWIO_SSC_RSCC_HIDDEN_TCS_STATUS_DRVd_TRIGGERED_SHFT                                                            0x0

#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR(d)                                                         (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000034 + 0x10000 * (d))
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_RMSK                                                            0xffffffff
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_MAXd                                                                     0
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR(d), HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_RMSK)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR_BMSK                                                       0xffffffff
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR_SHFT                                                              0x0

#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(d)                                                         (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000038 + 0x10000 * (d))
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_RMSK                                                            0x80ffffff
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_MAXd                                                                     0
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(d), HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_RMSK)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_OUTI(d,val)    \
        out_dword(HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(d),val)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(d),mask,val,HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_INI(d))
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_VALID_BMSK                                      0x80000000
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_VALID_SHFT                                            0x1f
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_HI_BMSK                                           0xffffff
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_HI_SHFT                                                0x0

#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR(d)                                                         (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x0000003c + 0x10000 * (d))
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_RMSK                                                            0xffffffff
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_MAXd                                                                     0
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR(d), HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_RMSK)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR_BMSK                                                       0xffffffff
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR_SHFT                                                              0x0

#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(d)                                                         (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000040 + 0x10000 * (d))
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_RMSK                                                            0xffffffff
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_MAXd                                                                     0
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(d), HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_RMSK)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_OUTI(d,val)    \
        out_dword(HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(d),val)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(d),mask,val,HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_INI(d))
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_PDC_MATCH_VALUE_LO_BMSK                                         0xffffffff
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_PDC_MATCH_VALUE_LO_SHFT                                                0x0

#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR(d)                                                         (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000044 + 0x10000 * (d))
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_RMSK                                                            0xffffffff
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_MAXd                                                                     0
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR(d), HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_RMSK)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR_BMSK                                                       0xffffffff
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR_SHFT                                                              0x0

#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR                                                            (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000048)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_RMSK                                                            0x8000ffff
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR, HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_RMSK)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR,m,v,HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_IN)
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_VALID_BMSK                                   0x80000000
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_VALID_SHFT                                         0x1f
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_BMSK                                             0xffff
#define HWIO_SSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_SHFT                                                0x0

#define HWIO_SSC_RSCC_HW_EVENT_OWNER_DRV0_ADDR                                                                  (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x0000004c)
#define HWIO_SSC_RSCC_HW_EVENT_OWNER_DRV0_RMSK                                                                         0x3
#define HWIO_SSC_RSCC_HW_EVENT_OWNER_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_HW_EVENT_OWNER_DRV0_ADDR, HWIO_SSC_RSCC_HW_EVENT_OWNER_DRV0_RMSK)
#define HWIO_SSC_RSCC_HW_EVENT_OWNER_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_HW_EVENT_OWNER_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_HW_EVENT_OWNER_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_HW_EVENT_OWNER_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_HW_EVENT_OWNER_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_HW_EVENT_OWNER_DRV0_ADDR,m,v,HWIO_SSC_RSCC_HW_EVENT_OWNER_DRV0_IN)
#define HWIO_SSC_RSCC_HW_EVENT_OWNER_DRV0_OWNER_BMSK                                                                   0x3
#define HWIO_SSC_RSCC_HW_EVENT_OWNER_DRV0_OWNER_SHFT                                                                   0x0

#define HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(d,m)                                                       (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000050 + 0x4 * (m) + 0x10000 * (d))
#define HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_RMSK                                                            0x80000007
#define HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_MAXd                                                                     0
#define HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_MAXm                                                                    31
#define HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_INI2(d,m)        \
        in_dword_masked(HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(d,m), HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_RMSK)
#define HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(d,m), mask)
#define HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_OUTI2(d,m,val)    \
        out_dword(HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(d,m),val)
#define HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_OUTMI2(d,m,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(d,m),mask,val,HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_INI2(d,m))
#define HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ENABLE_BMSK                                                     0x80000000
#define HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ENABLE_SHFT                                                           0x1f
#define HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_EVENT_SELECT_BMSK                                                      0x7
#define HWIO_SSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_EVENT_SELECT_SHFT                                                      0x0

#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_ADDR                                                            (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x000000d0)
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_RMSK                                                                   0x1
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_ADDR, HWIO_SSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_EPCB_TIMEOUT_IRQ_BMSK                                                  0x1
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_EPCB_TIMEOUT_IRQ_SHFT                                                  0x0

#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_ADDR                                                             (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x000000d4)
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_RMSK                                                                    0x1
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_CLR_EPCB_TIMEOUT_IRQ_BMSK                                               0x1
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_CLR_EPCB_TIMEOUT_IRQ_SHFT                                               0x0

#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR                                                            (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x000000d8)
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_RMSK                                                              0x10ffff
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR, HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR,m,v,HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_IN)
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_IRQ_EN_BMSK                                          0x100000
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_IRQ_EN_SHFT                                              0x14
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_THRESHOLD_BMSK                                         0xffff
#define HWIO_SSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_THRESHOLD_SHFT                                            0x0

#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR                                                        (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000200)
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_RMSK                                                               0x3
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR, HWIO_SSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR,m,v,HWIO_SSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_IN)
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OWNER_BMSK                                                         0x3
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OWNER_SHFT                                                         0x0

#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(d,m)                                                     (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000204 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_RMSK                                                                 0x1
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_MAXd                                                                   0
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_MAXm                                                                   5
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_INI2(d,m)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(d,m), HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_RMSK)
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(d,m), mask)
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_OUTI2(d,m,val)    \
        out_dword(HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(d,m),val)
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_OUTMI2(d,m,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(d,m),mask,val,HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_INI2(d,m))
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ENABLE_BMSK                                                          0x1
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ENABLE_SHFT                                                          0x0

#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(d,m)                                            (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000208 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_RMSK                                                 0xffffffff
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_MAXd                                                          0
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_MAXm                                                          5
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_INI2(d,m)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(d,m), HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_RMSK)
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(d,m), mask)
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_TS_DATA_L_BMSK                                       0xffffffff
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_TS_DATA_L_SHFT                                              0x0

#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(d,m)                                            (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x0000020c + 0x20 * (m) + 0x10000 * (d))
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_RMSK                                                   0xffffff
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_MAXd                                                          0
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_MAXm                                                          5
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_INI2(d,m)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(d,m), HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_RMSK)
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(d,m), mask)
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_TS_DATA_H_BMSK                                         0xffffff
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_TS_DATA_H_SHFT                                              0x0

#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(d,m)                                                 (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000210 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_RMSK                                                           0x101
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_MAXd                                                               0
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_MAXm                                                               5
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_INI2(d,m)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(d,m), HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_RMSK)
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(d,m), mask)
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_OVERFLOW_BMSK                                               0x100
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_OVERFLOW_SHFT                                                 0x8
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_VALID_BMSK                                                    0x1
#define HWIO_SSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_VALID_SHFT                                                    0x0

#define HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_ADDR(d)                                                        (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c00 + 0x10000 * (d))
#define HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_RMSK                                                           0xffffffff
#define HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_MAXd                                                                    0
#define HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_ADDR(d), HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_RMSK)
#define HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_OUTI(d,val)    \
        out_dword(HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_ADDR(d),val)
#define HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_ADDR(d),mask,val,HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_INI(d))
#define HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_TIMERS_CONSIDERED_ENABLE_MASK_BMSK                             0xffffffff
#define HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_TIMERS_CONSIDERED_ENABLE_MASK_SHFT                                    0x0

#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_ADDR(d)                                                      (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c04 + 0x10000 * (d))
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_RMSK                                                         0xffffffff
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_MAXd                                                                  0
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_ADDR(d), HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_RMSK)
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_OUTI(d,val)    \
        out_dword(HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_ADDR(d),val)
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_ADDR(d),mask,val,HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_INI(d))
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_SOFT_WAKEUP_TIME_LO_BMSK                                     0xffffffff
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_SOFT_WAKEUP_TIME_LO_SHFT                                            0x0

#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_ADDR(d)                                                      (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c08 + 0x10000 * (d))
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_RMSK                                                           0xffffff
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_MAXd                                                                  0
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_ADDR(d), HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_RMSK)
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_OUTI(d,val)    \
        out_dword(HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_ADDR(d),val)
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_ADDR(d),mask,val,HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_INI(d))
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_SOFT_WAKEUP_TIME_HI_BMSK                                       0xffffff
#define HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_SOFT_WAKEUP_TIME_HI_SHFT                                            0x0

#define HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_ADDR(d)                                                        (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c0c + 0x10000 * (d))
#define HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_RMSK                                                           0xffffffff
#define HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_MAXd                                                                    0
#define HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_ADDR(d), HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_RMSK)
#define HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_OUTI(d,val)    \
        out_dword(HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_ADDR(d),val)
#define HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_ADDR(d),mask,val,HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_INI(d))
#define HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_MAX_IDLE_DURATION_BMSK                                         0xffffffff
#define HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_MAX_IDLE_DURATION_SHFT                                                0x0

#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_ADDR                                                            (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c14)
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_RMSK                                                               0x10001
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_ADDR, HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_RMSK)
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_ADDR,m,v,HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_IN)
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_ONETIME_BMSK                                                       0x10000
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_ONETIME_SHFT                                                          0x10
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_CONTINUOUS_BMSK                                                        0x1
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_CONTINUOUS_SHFT                                                        0x0

#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_ADDR                                                            (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c18)
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_RMSK                                                                   0x7
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_ADDR, HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_RMSK)
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_ADDR,m,v,HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_IN)
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_MODE_BMSK                                                              0x7
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_MODE_SHFT                                                              0x0

#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_ADDR                                                       (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c1c)
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_RMSK                                                       0xffffffff
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_ADDR, HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_RMSK)
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_ADDR,m,v,HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_IN)
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_IDLE_TIME_BMSK                                             0xffffffff
#define HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_IDLE_TIME_SHFT                                                    0x0

#define HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_ADDR(d)                                                  (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c20 + 0x10000 * (d))
#define HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_RMSK                                                           0x1f
#define HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_MAXd                                                              0
#define HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_ADDR(d), HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_RMSK)
#define HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_OUTI(d,val)    \
        out_dword(HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_ADDR(d),val)
#define HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_ADDR(d),mask,val,HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_INI(d))
#define HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_MODE_ENABLE_MASK_BMSK                                          0x1e
#define HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_MODE_ENABLE_MASK_SHFT                                           0x1
#define HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_MODE_ENABLE_MASK_MODE0_BMSK                                     0x1
#define HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_MODE_ENABLE_MASK_MODE0_SHFT                                     0x0

#define HWIO_SSC_RSCC_SOLVER_STATUS0_DRVd_ADDR(d)                                                               (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c24 + 0x10000 * (d))
#define HWIO_SSC_RSCC_SOLVER_STATUS0_DRVd_RMSK                                                                         0x1
#define HWIO_SSC_RSCC_SOLVER_STATUS0_DRVd_MAXd                                                                           0
#define HWIO_SSC_RSCC_SOLVER_STATUS0_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_STATUS0_DRVd_ADDR(d), HWIO_SSC_RSCC_SOLVER_STATUS0_DRVd_RMSK)
#define HWIO_SSC_RSCC_SOLVER_STATUS0_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_STATUS0_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_SOLVER_STATUS0_DRVd_SOLVER_IN_OVERRIDE_BMSK                                                      0x1
#define HWIO_SSC_RSCC_SOLVER_STATUS0_DRVd_SOLVER_IN_OVERRIDE_SHFT                                                      0x0

#define HWIO_SSC_RSCC_SOLVER_STATUS1_DRVd_ADDR(d)                                                               (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c28 + 0x10000 * (d))
#define HWIO_SSC_RSCC_SOLVER_STATUS1_DRVd_RMSK                                                                         0x7
#define HWIO_SSC_RSCC_SOLVER_STATUS1_DRVd_MAXd                                                                           0
#define HWIO_SSC_RSCC_SOLVER_STATUS1_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_STATUS1_DRVd_ADDR(d), HWIO_SSC_RSCC_SOLVER_STATUS1_DRVd_RMSK)
#define HWIO_SSC_RSCC_SOLVER_STATUS1_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_STATUS1_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_SOLVER_STATUS1_DRVd_TIMESLOT_CHOSEN_BMSK                                                         0x7
#define HWIO_SSC_RSCC_SOLVER_STATUS1_DRVd_TIMESLOT_CHOSEN_SHFT                                                         0x0

#define HWIO_SSC_RSCC_SOLVER_STATUS2_DRVd_ADDR(d)                                                               (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c2c + 0x10000 * (d))
#define HWIO_SSC_RSCC_SOLVER_STATUS2_DRVd_RMSK                                                                         0x7
#define HWIO_SSC_RSCC_SOLVER_STATUS2_DRVd_MAXd                                                                           0
#define HWIO_SSC_RSCC_SOLVER_STATUS2_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_STATUS2_DRVd_ADDR(d), HWIO_SSC_RSCC_SOLVER_STATUS2_DRVd_RMSK)
#define HWIO_SSC_RSCC_SOLVER_STATUS2_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_STATUS2_DRVd_ADDR(d), mask)
#define HWIO_SSC_RSCC_SOLVER_STATUS2_DRVd_MODE_CHOSEN_BMSK                                                             0x7
#define HWIO_SSC_RSCC_SOLVER_STATUS2_DRVd_MODE_CHOSEN_SHFT                                                             0x0

#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_ADDR(m)                                                 (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c30 + 0x1C * (m))
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_RMSK                                                         0x3ff
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_MAXm                                                             3
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_INI(m)        \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_ADDR(m), HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_RMSK)
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_INMI(m,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_ADDR(m), mask)
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_OUTI(m,val)    \
        out_dword(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_ADDR(m),val)
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_ADDR(m),mask,val,HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_INI(m))
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_RSC_SEQ_START_ADDR_BMSK                                      0x3ff
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_RSC_SEQ_START_ADDR_SHFT                                        0x0

#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_ADDR(m)                                                 (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c34 + 0x1C * (m))
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_RMSK                                                    0xffffffff
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_MAXm                                                             3
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_INI(m)        \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_ADDR(m), HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_RMSK)
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_INMI(m,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_ADDR(m), mask)
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_OUTI(m,val)    \
        out_dword(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_ADDR(m),val)
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_ADDR(m),mask,val,HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_INI(m))
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_PDC_SEQ_VALID_BMSK                                      0x80000000
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_PDC_SEQ_VALID_SHFT                                            0x1f
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_PDC_TIMER_VALID_BMSK                                    0x40000000
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_PDC_TIMER_VALID_SHFT                                          0x1e
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_PDC_SEQ_START_ADDR_BMSK                                 0x3fffffff
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_PDC_SEQ_START_ADDR_SHFT                                        0x0

#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_ADDR(m)                                                 (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c38 + 0x1C * (m))
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_RMSK                                                    0xffffffff
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_MAXm                                                             3
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_INI(m)        \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_ADDR(m), HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_RMSK)
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_INMI(m,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_ADDR(m), mask)
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_OUTI(m,val)    \
        out_dword(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_ADDR(m),val)
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_ADDR(m),mask,val,HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_INI(m))
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_RSC_BACKOFF_TIME_BMSK                                   0xffffffff
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_RSC_BACKOFF_TIME_SHFT                                          0x0

#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_ADDR(m)                                                 (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c40 + 0x1C * (m))
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_RMSK                                                    0xffffffff
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_MAXm                                                             3
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_INI(m)        \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_ADDR(m), HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_RMSK)
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_INMI(m,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_ADDR(m), mask)
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_OUTI(m,val)    \
        out_dword(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_ADDR(m),val)
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_ADDR(m),mask,val,HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_INI(m))
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_PDC_BACKOFF_TIME_BMSK                                   0xffffffff
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_PDC_BACKOFF_TIME_SHFT                                          0x0

#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER4_DRV0_MODEm_ADDR(m)                                                 (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000c48 + 0x1C * (m))
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER4_DRV0_MODEm_RMSK                                                    0xffffffff
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER4_DRV0_MODEm_MAXm                                                             3
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER4_DRV0_MODEm_INI(m)        \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER4_DRV0_MODEm_ADDR(m), HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER4_DRV0_MODEm_RMSK)
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER4_DRV0_MODEm_INMI(m,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER4_DRV0_MODEm_ADDR(m), mask)
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER4_DRV0_MODEm_OUTI(m,val)    \
        out_dword(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER4_DRV0_MODEm_ADDR(m),val)
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER4_DRV0_MODEm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER4_DRV0_MODEm_ADDR(m),mask,val,HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER4_DRV0_MODEm_INI(m))
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER4_DRV0_MODEm_MODE_PARAM_GENERAL_PURPOSE_SET0_BMSK                    0xffffffff
#define HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER4_DRV0_MODEm_MODE_PARAM_GENERAL_PURPOSE_SET0_SHFT                           0x0

#define HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_ADDR(s)                                                     (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00001000 + 0x4 * (s))
#define HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_RMSK                                                        0xffffffff
#define HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_MAXs                                                                 3
#define HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_INI(s)        \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_ADDR(s), HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_RMSK)
#define HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_INMI(s,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_ADDR(s), mask)
#define HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_OUTI(s,val)    \
        out_dword(HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_ADDR(s),val)
#define HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_OUTMI(s,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_ADDR(s),mask,val,HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_INI(s))
#define HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_MIN_TIME_SLOT_BMSK                                          0xffffffff
#define HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_MIN_TIME_SLOT_SHFT                                                 0x0

#define HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_ADDR(s,p)                                 (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00001080 + 0x80 * (s) + 0x4 * (p))
#define HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_RMSK                                            0x1f
#define HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_MAXs                                               3
#define HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_MAXp                                               3
#define HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_INI2(s,p)        \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_ADDR(s,p), HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_RMSK)
#define HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_INMI2(s,p,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_ADDR(s,p), mask)
#define HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_OUTI2(s,p,val)    \
        out_dword(HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_ADDR(s,p),val)
#define HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_OUTMI2(s,p,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_ADDR(s,p),mask,val,HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_INI2(s,p))
#define HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_MODE_AT_PRIORITY_BMSK                           0x1f
#define HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_MODE_AT_PRIORITY_SHFT                            0x0

#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR                                                     (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000400)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_RMSK                                                     0x800003ff
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR, HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR,m,v,HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_IN)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_VALID_BMSK                                               0x80000000
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_VALID_SHFT                                                     0x1f
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR_BMSK                                                     0x3ff
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR_SHFT                                                       0x0

#define HWIO_SSC_RSCC_RSC_SEQ_BUSY_DRV0_ADDR                                                                    (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000404)
#define HWIO_SSC_RSCC_RSC_SEQ_BUSY_DRV0_RMSK                                                                           0x1
#define HWIO_SSC_RSCC_RSC_SEQ_BUSY_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_BUSY_DRV0_ADDR, HWIO_SSC_RSCC_RSC_SEQ_BUSY_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_SEQ_BUSY_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_BUSY_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_SEQ_BUSY_DRV0_BUSY_STAT_BMSK                                                                 0x1
#define HWIO_SSC_RSCC_RSC_SEQ_BUSY_DRV0_BUSY_STAT_SHFT                                                                 0x0

#define HWIO_SSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR                                                         (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000408)
#define HWIO_SSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_RMSK                                                              0x3ff
#define HWIO_SSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR, HWIO_SSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_CURRENT_INSTR_ADDR_BMSK                                           0x3ff
#define HWIO_SSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_CURRENT_INSTR_ADDR_SHFT                                             0x0

#define HWIO_SSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(i)                                                        (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000410 + 0x4 * (i))
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_RMSK                                                                0x3ff
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_MAXi                                                                    3
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INI(i)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(i), HWIO_SSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INMI(i,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(i), mask)
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_OUTI(i,val)    \
        out_dword(HWIO_SSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(i),val)
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_OUTMI(i,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(i),mask,val,HWIO_SSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INI(i))
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR_BMSK                                                           0x3ff
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR_SHFT                                                             0x0

#define HWIO_SSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(i)                                                      (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000450 + 0x4 * (i))
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_RMSK                                                            0xfffff
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_MAXi                                                                  3
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INI(i)        \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(i), HWIO_SSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INMI(i,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(i), mask)
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_OUTI(i,val)    \
        out_dword(HWIO_SSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(i),val)
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_OUTMI(i,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(i),mask,val,HWIO_SSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INI(i))
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_DELAY_VAL_BMSK                                                  0xfffff
#define HWIO_SSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_DELAY_VAL_SHFT                                                      0x0

#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR                                                        (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000460)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_RMSK                                                               0x1
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR, HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR,m,v,HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_IN)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_SEQ_OVERRIDE_TRIGGER_BMSK                                          0x1
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_SEQ_OVERRIDE_TRIGGER_SHFT                                          0x0

#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR                                          (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000464)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_RMSK                                               0x3ff
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR, HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR,m,v,HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_IN)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_SEQ_OVERRIDE_TRIGGER_START_ADDR_BMSK               0x3ff
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_SEQ_OVERRIDE_TRIGGER_START_ADDR_SHFT                 0x0

#define HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR                                                     (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000490)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_RMSK                                                     0x800003ff
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR, HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR,m,v,HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_IN)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_VALID_BMSK                                0x80000000
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_VALID_SHFT                                      0x1f
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_ADDR_BMSK                                      0x3ff
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_ADDR_SHFT                                        0x0

#define HWIO_SSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR                                                                (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000494)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_RMSK                                                                       0x1
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR, HWIO_SSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR,m,v,HWIO_SSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_IN)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_DBG_STEP_TRIGGER_BMSK                                                      0x1
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_DBG_STEP_TRIGGER_SHFT                                                      0x0

#define HWIO_SSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR                                                            (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000498)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_RMSK                                                                   0x1
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR, HWIO_SSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR,m,v,HWIO_SSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_IN)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_DBG_STEP_CONTINUE_BMSK                                                 0x1
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_DBG_STEP_CONTINUE_SHFT                                                 0x0

#define HWIO_SSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_ADDR                                                                (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x0000049c)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_RMSK                                                                       0x1
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_ADDR, HWIO_SSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_DBG_STEPPING_STATUS_BMSK                                                   0x1
#define HWIO_SSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_DBG_STEPPING_STATUS_SHFT                                                   0x0

#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR                                                 (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x000004a0)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_RMSK                                                       0x3f
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR, HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR,m,v,HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_IN)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_SEQ_OVERRIDE_PWR_CNTRL_MASK_BMSK                           0x3f
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_SEQ_OVERRIDE_PWR_CNTRL_MASK_SHFT                            0x0

#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR                                                  (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x000004a4)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_RMSK                                                        0x3f
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR, HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR,m,v,HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_IN)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_SEQ_OVERRIDE_PWR_CNTRL_VAL_BMSK                             0x3f
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_SEQ_OVERRIDE_PWR_CNTRL_VAL_SHFT                              0x0

#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR                                                (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x000004a8)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_RMSK                                                      0x3f
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR, HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR,m,v,HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_IN)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_SEQ_OVERRIDE_WAIT_EVENT_MASK_BMSK                         0x3f
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_SEQ_OVERRIDE_WAIT_EVENT_MASK_SHFT                          0x0

#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR                                                 (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x000004ac)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_RMSK                                                       0x3f
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR, HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_RMSK)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR,m,v,HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_IN)
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_SEQ_OVERRIDE_WAIT_EVENT_VAL_BMSK                           0x3f
#define HWIO_SSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_SEQ_OVERRIDE_WAIT_EVENT_VAL_SHFT                            0x0

#define HWIO_SSC_RSCC_SEQ_MEM_m_DRV0_ADDR(m)                                                                    (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00000600 + 0x4 * (m))
#define HWIO_SSC_RSCC_SEQ_MEM_m_DRV0_RMSK                                                                       0xffffffff
#define HWIO_SSC_RSCC_SEQ_MEM_m_DRV0_MAXm                                                                               23
#define HWIO_SSC_RSCC_SEQ_MEM_m_DRV0_INI(m)        \
        in_dword_masked(HWIO_SSC_RSCC_SEQ_MEM_m_DRV0_ADDR(m), HWIO_SSC_RSCC_SEQ_MEM_m_DRV0_RMSK)
#define HWIO_SSC_RSCC_SEQ_MEM_m_DRV0_INMI(m,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_SEQ_MEM_m_DRV0_ADDR(m), mask)
#define HWIO_SSC_RSCC_SEQ_MEM_m_DRV0_OUTI(m,val)    \
        out_dword(HWIO_SSC_RSCC_SEQ_MEM_m_DRV0_ADDR(m),val)
#define HWIO_SSC_RSCC_SEQ_MEM_m_DRV0_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_SEQ_MEM_m_DRV0_ADDR(m),mask,val,HWIO_SSC_RSCC_SEQ_MEM_m_DRV0_INI(m))
#define HWIO_SSC_RSCC_SEQ_MEM_m_DRV0_CMD_WORD_BMSK                                                              0xffffffff
#define HWIO_SSC_RSCC_SEQ_MEM_m_DRV0_CMD_WORD_SHFT                                                                     0x0

#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR                                                         (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00001c00)
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_RMSK                                                               0x3f
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR, HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_RMSK)
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR,m,v,HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_IN)
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_AMC_COMPLETION_IRQ_ENABLE_BMSK                                     0x3f
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_AMC_COMPLETION_IRQ_ENABLE_SHFT                                      0x0

#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_ADDR                                                         (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00001c04)
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_RMSK                                                               0x3f
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_IN          \
        in_dword_masked(HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_ADDR, HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_RMSK)
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_ADDR, m)
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_AMC_COMPLETION_IRQ_STATUS_BMSK                                     0x3f
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_AMC_COMPLETION_IRQ_STATUS_SHFT                                      0x0

#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_ADDR                                                          (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00001c08)
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_RMSK                                                                0x3f
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_ADDR,v)
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_AMC_COMPLETION_IRQ_CLEAR_BMSK                                       0x3f
#define HWIO_SSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_AMC_COMPLETION_IRQ_CLEAR_SHFT                                        0x0

#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(m)                                                       (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00001c10 + 0x2A0 * (m))
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_RMSK                                                              0xffff
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_MAXm                                                                   5
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_INI(m)        \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(m), HWIO_SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_RMSK)
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_INMI(m,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(m), mask)
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_OUTI(m,val)    \
        out_dword(HWIO_SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(m),val)
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(m),mask,val,HWIO_SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_INI(m))
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_BMSK                                            0xffff
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_SHFT                                               0x0

#define HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_ADDR(m)                                                                 (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00001c14 + 0x2A0 * (m))
#define HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_RMSK                                                                     0x1010000
#define HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_MAXm                                                                             5
#define HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_INI(m)        \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_ADDR(m), HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_RMSK)
#define HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_INMI(m,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_ADDR(m), mask)
#define HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_OUTI(m,val)    \
        out_dword(HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_ADDR(m),val)
#define HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_ADDR(m),mask,val,HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_INI(m))
#define HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_AMC_MODE_TRIGGER_BMSK                                                    0x1000000
#define HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_AMC_MODE_TRIGGER_SHFT                                                         0x18
#define HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_AMC_MODE_EN_BMSK                                                           0x10000
#define HWIO_SSC_RSCC_TCSm_DRV0_CONTROL_AMC_MODE_EN_SHFT                                                              0x10

#define HWIO_SSC_RSCC_TCSm_DRV0_STATUS_ADDR(m)                                                                  (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00001c18 + 0x2A0 * (m))
#define HWIO_SSC_RSCC_TCSm_DRV0_STATUS_RMSK                                                                            0x1
#define HWIO_SSC_RSCC_TCSm_DRV0_STATUS_MAXm                                                                              5
#define HWIO_SSC_RSCC_TCSm_DRV0_STATUS_INI(m)        \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_DRV0_STATUS_ADDR(m), HWIO_SSC_RSCC_TCSm_DRV0_STATUS_RMSK)
#define HWIO_SSC_RSCC_TCSm_DRV0_STATUS_INMI(m,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_DRV0_STATUS_ADDR(m), mask)
#define HWIO_SSC_RSCC_TCSm_DRV0_STATUS_CONTROLLER_IDLE_BMSK                                                            0x1
#define HWIO_SSC_RSCC_TCSm_DRV0_STATUS_CONTROLLER_IDLE_SHFT                                                            0x0

#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_ENABLE_ADDR(m)                                                              (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00001c1c + 0x2A0 * (m))
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_ENABLE_RMSK                                                                     0xffff
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_ENABLE_MAXm                                                                          5
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_ENABLE_INI(m)        \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_DRV0_CMD_ENABLE_ADDR(m), HWIO_SSC_RSCC_TCSm_DRV0_CMD_ENABLE_RMSK)
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_ENABLE_INMI(m,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_DRV0_CMD_ENABLE_ADDR(m), mask)
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_ENABLE_OUTI(m,val)    \
        out_dword(HWIO_SSC_RSCC_TCSm_DRV0_CMD_ENABLE_ADDR(m),val)
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_ENABLE_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_TCSm_DRV0_CMD_ENABLE_ADDR(m),mask,val,HWIO_SSC_RSCC_TCSm_DRV0_CMD_ENABLE_INI(m))
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_ENABLE_INDIVIDUAL_CMD_ENABLE_MASK_BMSK                                          0xffff
#define HWIO_SSC_RSCC_TCSm_DRV0_CMD_ENABLE_INDIVIDUAL_CMD_ENABLE_MASK_SHFT                                             0x0

#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_ADDR(m,n)                                                            (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00001c30 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_RMSK                                                                    0x1010f
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_MAXm                                                                          5
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_MAXn                                                                         15
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_INI2(m,n)        \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_ADDR(m,n), HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_RMSK)
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_ADDR(m,n), mask)
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_OUTI2(m,n,val)    \
        out_dword(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_ADDR(m,n),val)
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_OUTMI2(m,n,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_ADDR(m,n),mask,val,HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_INI2(m,n))
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_READ_OR_WRITE_BMSK                                                      0x10000
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_READ_OR_WRITE_SHFT                                                         0x10
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_RES_REQ_BMSK                                                              0x100
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_RES_REQ_SHFT                                                                0x8
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_MSG_LENGTH_BMSK                                                             0xf
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_MSGID_MSG_LENGTH_SHFT                                                             0x0

#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_ADDR(m,n)                                                             (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00001c34 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_RMSK                                                                     0x7ffff
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_MAXm                                                                           5
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_MAXn                                                                          15
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_INI2(m,n)        \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_ADDR(m,n), HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_RMSK)
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_ADDR(m,n), mask)
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_OUTI2(m,n,val)    \
        out_dword(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_ADDR(m,n),val)
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_OUTMI2(m,n,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_ADDR(m,n),mask,val,HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_INI2(m,n))
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_SLV_ID_BMSK                                                              0x70000
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_SLV_ID_SHFT                                                                 0x10
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_OFFSET_BMSK                                                               0xffff
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_ADDR_OFFSET_SHFT                                                                  0x0

#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_ADDR(m,n)                                                             (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00001c38 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_RMSK                                                                  0xffffffff
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_MAXm                                                                           5
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_MAXn                                                                          15
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_INI2(m,n)        \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_ADDR(m,n), HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_RMSK)
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_ADDR(m,n), mask)
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_OUTI2(m,n,val)    \
        out_dword(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_ADDR(m,n),val)
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_OUTMI2(m,n,mask,val) \
        out_dword_masked_ns(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_ADDR(m,n),mask,val,HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_INI2(m,n))
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_DATA_BMSK                                                             0xffffffff
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_DATA_DATA_SHFT                                                                    0x0

#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_STATUS_ADDR(m,n)                                                           (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00001c3c + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_STATUS_RMSK                                                                   0x10101
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_STATUS_MAXm                                                                         5
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_STATUS_MAXn                                                                        15
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_STATUS_INI2(m,n)        \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_STATUS_ADDR(m,n), HWIO_SSC_RSCC_TCSm_CMDn_DRV0_STATUS_RMSK)
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_STATUS_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_STATUS_ADDR(m,n), mask)
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_STATUS_COMPLETED_BMSK                                                         0x10000
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_STATUS_COMPLETED_SHFT                                                            0x10
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_STATUS_ISSUED_BMSK                                                              0x100
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_STATUS_ISSUED_SHFT                                                                0x8
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_STATUS_TRIGGERED_BMSK                                                             0x1
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_STATUS_TRIGGERED_SHFT                                                             0x0

#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_ADDR(m,n)                                               (SSC_RSCC_RSCC_RSC_REG_BASE      + 0x00001c40 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_RMSK                                                    0xffffffff
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_MAXm                                                             5
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_MAXn                                                            15
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_INI2(m,n)        \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_ADDR(m,n), HWIO_SSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_RMSK)
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_SSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_ADDR(m,n), mask)
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_READ_RESPONSE_DATA_BMSK                                 0xffffffff
#define HWIO_SSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_READ_RESPONSE_DATA_SHFT                                        0x0

/*----------------------------------------------------------------------------
 * MODULE: SSC_QTMR_AC
 *--------------------------------------------------------------------------*/

#define SSC_QTMR_AC_REG_BASE                                        (LPASS_BASE      + 0x00ba8000)

#define HWIO_SSC_QTMR_AC_CNTFRQ_ADDR                                (SSC_QTMR_AC_REG_BASE      + 0x00000000)
#define HWIO_SSC_QTMR_AC_CNTFRQ_RMSK                                0xffffffff
#define HWIO_SSC_QTMR_AC_CNTFRQ_IN          \
        in_dword_masked(HWIO_SSC_QTMR_AC_CNTFRQ_ADDR, HWIO_SSC_QTMR_AC_CNTFRQ_RMSK)
#define HWIO_SSC_QTMR_AC_CNTFRQ_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_AC_CNTFRQ_ADDR, m)
#define HWIO_SSC_QTMR_AC_CNTFRQ_OUT(v)      \
        out_dword(HWIO_SSC_QTMR_AC_CNTFRQ_ADDR,v)
#define HWIO_SSC_QTMR_AC_CNTFRQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QTMR_AC_CNTFRQ_ADDR,m,v,HWIO_SSC_QTMR_AC_CNTFRQ_IN)
#define HWIO_SSC_QTMR_AC_CNTFRQ_CNTFRQ_BMSK                         0xffffffff
#define HWIO_SSC_QTMR_AC_CNTFRQ_CNTFRQ_SHFT                                0x0

#define HWIO_SSC_QTMR_AC_CNTNSAR_FG0_ADDR                           (SSC_QTMR_AC_REG_BASE      + 0x00000004)
#define HWIO_SSC_QTMR_AC_CNTNSAR_FG0_RMSK                                  0x3
#define HWIO_SSC_QTMR_AC_CNTNSAR_FG0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_AC_CNTNSAR_FG0_ADDR, HWIO_SSC_QTMR_AC_CNTNSAR_FG0_RMSK)
#define HWIO_SSC_QTMR_AC_CNTNSAR_FG0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_AC_CNTNSAR_FG0_ADDR, m)
#define HWIO_SSC_QTMR_AC_CNTNSAR_FG0_OUT(v)      \
        out_dword(HWIO_SSC_QTMR_AC_CNTNSAR_FG0_ADDR,v)
#define HWIO_SSC_QTMR_AC_CNTNSAR_FG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QTMR_AC_CNTNSAR_FG0_ADDR,m,v,HWIO_SSC_QTMR_AC_CNTNSAR_FG0_IN)
#define HWIO_SSC_QTMR_AC_CNTNSAR_FG0_NSN_BMSK                              0x3
#define HWIO_SSC_QTMR_AC_CNTNSAR_FG0_NSN_SHFT                              0x0

#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_ADDR                           (SSC_QTMR_AC_REG_BASE      + 0x00000008)
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_RMSK                           0xffffffff
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_AC_CNTTIDR_FG0_ADDR, HWIO_SSC_QTMR_AC_CNTTIDR_FG0_RMSK)
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_AC_CNTTIDR_FG0_ADDR, m)
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F7_CFG_BMSK                    0xf0000000
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F7_CFG_SHFT                          0x1c
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F6_CFG_BMSK                     0xf000000
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F6_CFG_SHFT                          0x18
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F5_CFG_BMSK                      0xf00000
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F5_CFG_SHFT                          0x14
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F4_CFG_BMSK                       0xf0000
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F4_CFG_SHFT                          0x10
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F3_CFG_BMSK                        0xf000
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F3_CFG_SHFT                           0xc
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F2_CFG_BMSK                         0xf00
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F2_CFG_SHFT                           0x8
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F1_CFG_BMSK                          0xf0
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F1_CFG_SHFT                           0x4
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F0_CFG_BMSK                           0xf
#define HWIO_SSC_QTMR_AC_CNTTIDR_FG0_F0_CFG_SHFT                           0x0

#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_ADDR(n)                        (SSC_QTMR_AC_REG_BASE      + 0x00000040 + 0x4 * (n))
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_RMSK                                 0x3f
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_MAXn                                    1
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_INI(n)        \
        in_dword_masked(HWIO_SSC_QTMR_AC_CNTACRn_FG0_ADDR(n), HWIO_SSC_QTMR_AC_CNTACRn_FG0_RMSK)
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_INMI(n,mask)    \
        in_dword_masked(HWIO_SSC_QTMR_AC_CNTACRn_FG0_ADDR(n), mask)
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_OUTI(n,val)    \
        out_dword(HWIO_SSC_QTMR_AC_CNTACRn_FG0_ADDR(n),val)
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SSC_QTMR_AC_CNTACRn_FG0_ADDR(n),mask,val,HWIO_SSC_QTMR_AC_CNTACRn_FG0_INI(n))
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_RWPT_BMSK                            0x20
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_RWPT_SHFT                             0x5
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_RWVT_BMSK                            0x10
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_RWVT_SHFT                             0x4
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_RVOFF_BMSK                            0x8
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_RVOFF_SHFT                            0x3
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_RFRQ_BMSK                             0x4
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_RFRQ_SHFT                             0x2
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_RPVCT_BMSK                            0x2
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_RPVCT_SHFT                            0x1
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_RPCT_BMSK                             0x1
#define HWIO_SSC_QTMR_AC_CNTACRn_FG0_RPCT_SHFT                             0x0

#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_LO_n_ADDR(n)                   (SSC_QTMR_AC_REG_BASE      + 0x00000080 + 0x8 * (n))
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_LO_n_RMSK                      0xffffffff
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_LO_n_MAXn                               1
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_LO_n_INI(n)        \
        in_dword_masked(HWIO_SSC_QTMR_AC_CNTVOFF_FG0_LO_n_ADDR(n), HWIO_SSC_QTMR_AC_CNTVOFF_FG0_LO_n_RMSK)
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_LO_n_INMI(n,mask)    \
        in_dword_masked(HWIO_SSC_QTMR_AC_CNTVOFF_FG0_LO_n_ADDR(n), mask)
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_LO_n_OUTI(n,val)    \
        out_dword(HWIO_SSC_QTMR_AC_CNTVOFF_FG0_LO_n_ADDR(n),val)
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_LO_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SSC_QTMR_AC_CNTVOFF_FG0_LO_n_ADDR(n),mask,val,HWIO_SSC_QTMR_AC_CNTVOFF_FG0_LO_n_INI(n))
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_LO_n_CNTVOFF_LO_BMSK           0xffffffff
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_LO_n_CNTVOFF_LO_SHFT                  0x0

#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_HI_n_ADDR(n)                   (SSC_QTMR_AC_REG_BASE      + 0x00000084 + 0x8 * (n))
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_HI_n_RMSK                        0xffffff
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_HI_n_MAXn                               1
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_HI_n_INI(n)        \
        in_dword_masked(HWIO_SSC_QTMR_AC_CNTVOFF_FG0_HI_n_ADDR(n), HWIO_SSC_QTMR_AC_CNTVOFF_FG0_HI_n_RMSK)
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_HI_n_INMI(n,mask)    \
        in_dword_masked(HWIO_SSC_QTMR_AC_CNTVOFF_FG0_HI_n_ADDR(n), mask)
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_HI_n_OUTI(n,val)    \
        out_dword(HWIO_SSC_QTMR_AC_CNTVOFF_FG0_HI_n_ADDR(n),val)
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_HI_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SSC_QTMR_AC_CNTVOFF_FG0_HI_n_ADDR(n),mask,val,HWIO_SSC_QTMR_AC_CNTVOFF_FG0_HI_n_INI(n))
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_HI_n_CNTVOFF_HI_BMSK             0xffffff
#define HWIO_SSC_QTMR_AC_CNTVOFF_FG0_HI_n_CNTVOFF_HI_SHFT                  0x0

#define HWIO_SSC_QTMR_AC_CFG_ADDR                                   (SSC_QTMR_AC_REG_BASE      + 0x00000fc0)
#define HWIO_SSC_QTMR_AC_CFG_RMSK                                          0x3
#define HWIO_SSC_QTMR_AC_CFG_IN          \
        in_dword_masked(HWIO_SSC_QTMR_AC_CFG_ADDR, HWIO_SSC_QTMR_AC_CFG_RMSK)
#define HWIO_SSC_QTMR_AC_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_AC_CFG_ADDR, m)
#define HWIO_SSC_QTMR_AC_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QTMR_AC_CFG_ADDR,v)
#define HWIO_SSC_QTMR_AC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QTMR_AC_CFG_ADDR,m,v,HWIO_SSC_QTMR_AC_CFG_IN)
#define HWIO_SSC_QTMR_AC_CFG_DSBL_ATOMIC_BMSK                              0x2
#define HWIO_SSC_QTMR_AC_CFG_DSBL_ATOMIC_SHFT                              0x1
#define HWIO_SSC_QTMR_AC_CFG_TEST_BUS_EN_BMSK                              0x1
#define HWIO_SSC_QTMR_AC_CFG_TEST_BUS_EN_SHFT                              0x0

#define HWIO_SSC_QTMR_AC_VERSION_ADDR                               (SSC_QTMR_AC_REG_BASE      + 0x00000fd0)
#define HWIO_SSC_QTMR_AC_VERSION_RMSK                               0xffffffff
#define HWIO_SSC_QTMR_AC_VERSION_IN          \
        in_dword_masked(HWIO_SSC_QTMR_AC_VERSION_ADDR, HWIO_SSC_QTMR_AC_VERSION_RMSK)
#define HWIO_SSC_QTMR_AC_VERSION_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_AC_VERSION_ADDR, m)
#define HWIO_SSC_QTMR_AC_VERSION_MAJOR_BMSK                         0xf0000000
#define HWIO_SSC_QTMR_AC_VERSION_MAJOR_SHFT                               0x1c
#define HWIO_SSC_QTMR_AC_VERSION_MINOR_BMSK                          0xfff0000
#define HWIO_SSC_QTMR_AC_VERSION_MINOR_SHFT                               0x10
#define HWIO_SSC_QTMR_AC_VERSION_STEP_BMSK                              0xffff
#define HWIO_SSC_QTMR_AC_VERSION_STEP_SHFT                                 0x0

#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_1_ADDR                        (SSC_QTMR_AC_REG_BASE      + 0x00000fe0)
#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_1_RMSK                        0xffffffff
#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_1_IN          \
        in_dword_masked(HWIO_SSC_QTMR_AC_HW_FRAME_SEL_1_ADDR, HWIO_SSC_QTMR_AC_HW_FRAME_SEL_1_RMSK)
#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_AC_HW_FRAME_SEL_1_ADDR, m)
#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_1_OUT(v)      \
        out_dword(HWIO_SSC_QTMR_AC_HW_FRAME_SEL_1_ADDR,v)
#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QTMR_AC_HW_FRAME_SEL_1_ADDR,m,v,HWIO_SSC_QTMR_AC_HW_FRAME_SEL_1_IN)
#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_1_HW_FRAME_SEL1_BMSK          0xffffffff
#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_1_HW_FRAME_SEL1_SHFT                 0x0

#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_ADDR                        (SSC_QTMR_AC_REG_BASE      + 0x00000ff0)
#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_RMSK                        0xffffffff
#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_IN          \
        in_dword_masked(HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_ADDR, HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_RMSK)
#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_ADDR, m)
#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_OUT(v)      \
        out_dword(HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_ADDR,v)
#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_ADDR,m,v,HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_IN)
#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_HW_FRAME_SEL2_BMSK          0xffffffff
#define HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_HW_FRAME_SEL2_SHFT                 0x0

/*----------------------------------------------------------------------------
 * MODULE: SSC_QTMR_F0_0
 *--------------------------------------------------------------------------*/

#define SSC_QTMR_F0_0_REG_BASE                                     (LPASS_BASE      + 0x00ba9000)

#define HWIO_SSC_QTMR_V1_CNTPCT_LO_0_ADDR                          (SSC_QTMR_F0_0_REG_BASE      + 0x00000000)
#define HWIO_SSC_QTMR_V1_CNTPCT_LO_0_RMSK                          0xffffffff
#define HWIO_SSC_QTMR_V1_CNTPCT_LO_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTPCT_LO_0_ADDR, HWIO_SSC_QTMR_V1_CNTPCT_LO_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTPCT_LO_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTPCT_LO_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTPCT_LO_0_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_SSC_QTMR_V1_CNTPCT_LO_0_CNTPCT_LO_SHFT                       0x0

#define HWIO_SSC_QTMR_V1_CNTPCT_HI_0_ADDR                          (SSC_QTMR_F0_0_REG_BASE      + 0x00000004)
#define HWIO_SSC_QTMR_V1_CNTPCT_HI_0_RMSK                            0xffffff
#define HWIO_SSC_QTMR_V1_CNTPCT_HI_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTPCT_HI_0_ADDR, HWIO_SSC_QTMR_V1_CNTPCT_HI_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTPCT_HI_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTPCT_HI_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTPCT_HI_0_CNTPCT_HI_BMSK                  0xffffff
#define HWIO_SSC_QTMR_V1_CNTPCT_HI_0_CNTPCT_HI_SHFT                       0x0

#define HWIO_SSC_QTMR_V1_CNTVCT_LO_0_ADDR                          (SSC_QTMR_F0_0_REG_BASE      + 0x00000008)
#define HWIO_SSC_QTMR_V1_CNTVCT_LO_0_RMSK                          0xffffffff
#define HWIO_SSC_QTMR_V1_CNTVCT_LO_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTVCT_LO_0_ADDR, HWIO_SSC_QTMR_V1_CNTVCT_LO_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTVCT_LO_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTVCT_LO_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTVCT_LO_0_CNTVCT_LO_BMSK                0xffffffff
#define HWIO_SSC_QTMR_V1_CNTVCT_LO_0_CNTVCT_LO_SHFT                       0x0

#define HWIO_SSC_QTMR_V1_CNTVCT_HI_0_ADDR                          (SSC_QTMR_F0_0_REG_BASE      + 0x0000000c)
#define HWIO_SSC_QTMR_V1_CNTVCT_HI_0_RMSK                            0xffffff
#define HWIO_SSC_QTMR_V1_CNTVCT_HI_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTVCT_HI_0_ADDR, HWIO_SSC_QTMR_V1_CNTVCT_HI_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTVCT_HI_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTVCT_HI_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTVCT_HI_0_CNTVCT_HI_BMSK                  0xffffff
#define HWIO_SSC_QTMR_V1_CNTVCT_HI_0_CNTVCT_HI_SHFT                       0x0

#define HWIO_SSC_QTMR_V1_CNTFRQ_0_ADDR                             (SSC_QTMR_F0_0_REG_BASE      + 0x00000010)
#define HWIO_SSC_QTMR_V1_CNTFRQ_0_RMSK                             0xffffffff
#define HWIO_SSC_QTMR_V1_CNTFRQ_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTFRQ_0_ADDR, HWIO_SSC_QTMR_V1_CNTFRQ_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTFRQ_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTFRQ_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTFRQ_0_CNTFRQ_BMSK                      0xffffffff
#define HWIO_SSC_QTMR_V1_CNTFRQ_0_CNTFRQ_SHFT                             0x0

#define HWIO_SSC_QTMR_V1_CNTPL0ACR_0_ADDR                          (SSC_QTMR_F0_0_REG_BASE      + 0x00000014)
#define HWIO_SSC_QTMR_V1_CNTPL0ACR_0_RMSK                               0x303
#define HWIO_SSC_QTMR_V1_CNTPL0ACR_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTPL0ACR_0_ADDR, HWIO_SSC_QTMR_V1_CNTPL0ACR_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTPL0ACR_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTPL0ACR_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTPL0ACR_0_OUT(v)      \
        out_dword(HWIO_SSC_QTMR_V1_CNTPL0ACR_0_ADDR,v)
#define HWIO_SSC_QTMR_V1_CNTPL0ACR_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QTMR_V1_CNTPL0ACR_0_ADDR,m,v,HWIO_SSC_QTMR_V1_CNTPL0ACR_0_IN)
#define HWIO_SSC_QTMR_V1_CNTPL0ACR_0_PL0CTEN_BMSK                       0x200
#define HWIO_SSC_QTMR_V1_CNTPL0ACR_0_PL0CTEN_SHFT                         0x9
#define HWIO_SSC_QTMR_V1_CNTPL0ACR_0_PL0VTEN_BMSK                       0x100
#define HWIO_SSC_QTMR_V1_CNTPL0ACR_0_PL0VTEN_SHFT                         0x8
#define HWIO_SSC_QTMR_V1_CNTPL0ACR_0_PL0VCTEN_BMSK                        0x2
#define HWIO_SSC_QTMR_V1_CNTPL0ACR_0_PL0VCTEN_SHFT                        0x1
#define HWIO_SSC_QTMR_V1_CNTPL0ACR_0_PL0PCTEN_BMSK                        0x1
#define HWIO_SSC_QTMR_V1_CNTPL0ACR_0_PL0PCTEN_SHFT                        0x0

#define HWIO_SSC_QTMR_V1_CNTVOFF_LO_0_ADDR                         (SSC_QTMR_F0_0_REG_BASE      + 0x00000018)
#define HWIO_SSC_QTMR_V1_CNTVOFF_LO_0_RMSK                         0xffffffff
#define HWIO_SSC_QTMR_V1_CNTVOFF_LO_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTVOFF_LO_0_ADDR, HWIO_SSC_QTMR_V1_CNTVOFF_LO_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTVOFF_LO_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTVOFF_LO_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTVOFF_LO_0_CNTVOFF_L0_BMSK              0xffffffff
#define HWIO_SSC_QTMR_V1_CNTVOFF_LO_0_CNTVOFF_L0_SHFT                     0x0

#define HWIO_SSC_QTMR_V1_CNTVOFF_HI_0_ADDR                         (SSC_QTMR_F0_0_REG_BASE      + 0x0000001c)
#define HWIO_SSC_QTMR_V1_CNTVOFF_HI_0_RMSK                           0xffffff
#define HWIO_SSC_QTMR_V1_CNTVOFF_HI_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTVOFF_HI_0_ADDR, HWIO_SSC_QTMR_V1_CNTVOFF_HI_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTVOFF_HI_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTVOFF_HI_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTVOFF_HI_0_CNTVOFF_HI_BMSK                0xffffff
#define HWIO_SSC_QTMR_V1_CNTVOFF_HI_0_CNTVOFF_HI_SHFT                     0x0

#define HWIO_SSC_QTMR_V1_CNTP_CVAL_LO_0_ADDR                       (SSC_QTMR_F0_0_REG_BASE      + 0x00000020)
#define HWIO_SSC_QTMR_V1_CNTP_CVAL_LO_0_RMSK                       0xffffffff
#define HWIO_SSC_QTMR_V1_CNTP_CVAL_LO_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTP_CVAL_LO_0_ADDR, HWIO_SSC_QTMR_V1_CNTP_CVAL_LO_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTP_CVAL_LO_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTP_CVAL_LO_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTP_CVAL_LO_0_OUT(v)      \
        out_dword(HWIO_SSC_QTMR_V1_CNTP_CVAL_LO_0_ADDR,v)
#define HWIO_SSC_QTMR_V1_CNTP_CVAL_LO_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QTMR_V1_CNTP_CVAL_LO_0_ADDR,m,v,HWIO_SSC_QTMR_V1_CNTP_CVAL_LO_0_IN)
#define HWIO_SSC_QTMR_V1_CNTP_CVAL_LO_0_CNTP_CVAL_L0_BMSK          0xffffffff
#define HWIO_SSC_QTMR_V1_CNTP_CVAL_LO_0_CNTP_CVAL_L0_SHFT                 0x0

#define HWIO_SSC_QTMR_V1_CNTP_CVAL_HI_0_ADDR                       (SSC_QTMR_F0_0_REG_BASE      + 0x00000024)
#define HWIO_SSC_QTMR_V1_CNTP_CVAL_HI_0_RMSK                         0xffffff
#define HWIO_SSC_QTMR_V1_CNTP_CVAL_HI_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTP_CVAL_HI_0_ADDR, HWIO_SSC_QTMR_V1_CNTP_CVAL_HI_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTP_CVAL_HI_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTP_CVAL_HI_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTP_CVAL_HI_0_OUT(v)      \
        out_dword(HWIO_SSC_QTMR_V1_CNTP_CVAL_HI_0_ADDR,v)
#define HWIO_SSC_QTMR_V1_CNTP_CVAL_HI_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QTMR_V1_CNTP_CVAL_HI_0_ADDR,m,v,HWIO_SSC_QTMR_V1_CNTP_CVAL_HI_0_IN)
#define HWIO_SSC_QTMR_V1_CNTP_CVAL_HI_0_CNTP_CVAL_HI_BMSK            0xffffff
#define HWIO_SSC_QTMR_V1_CNTP_CVAL_HI_0_CNTP_CVAL_HI_SHFT                 0x0

#define HWIO_SSC_QTMR_V1_CNTP_TVAL_0_ADDR                          (SSC_QTMR_F0_0_REG_BASE      + 0x00000028)
#define HWIO_SSC_QTMR_V1_CNTP_TVAL_0_RMSK                          0xffffffff
#define HWIO_SSC_QTMR_V1_CNTP_TVAL_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTP_TVAL_0_ADDR, HWIO_SSC_QTMR_V1_CNTP_TVAL_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTP_TVAL_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTP_TVAL_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTP_TVAL_0_OUT(v)      \
        out_dword(HWIO_SSC_QTMR_V1_CNTP_TVAL_0_ADDR,v)
#define HWIO_SSC_QTMR_V1_CNTP_TVAL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QTMR_V1_CNTP_TVAL_0_ADDR,m,v,HWIO_SSC_QTMR_V1_CNTP_TVAL_0_IN)
#define HWIO_SSC_QTMR_V1_CNTP_TVAL_0_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_SSC_QTMR_V1_CNTP_TVAL_0_CNTP_TVAL_SHFT                       0x0

#define HWIO_SSC_QTMR_V1_CNTP_CTL_0_ADDR                           (SSC_QTMR_F0_0_REG_BASE      + 0x0000002c)
#define HWIO_SSC_QTMR_V1_CNTP_CTL_0_RMSK                                  0x7
#define HWIO_SSC_QTMR_V1_CNTP_CTL_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTP_CTL_0_ADDR, HWIO_SSC_QTMR_V1_CNTP_CTL_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTP_CTL_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTP_CTL_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTP_CTL_0_OUT(v)      \
        out_dword(HWIO_SSC_QTMR_V1_CNTP_CTL_0_ADDR,v)
#define HWIO_SSC_QTMR_V1_CNTP_CTL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QTMR_V1_CNTP_CTL_0_ADDR,m,v,HWIO_SSC_QTMR_V1_CNTP_CTL_0_IN)
#define HWIO_SSC_QTMR_V1_CNTP_CTL_0_ISTAT_BMSK                            0x4
#define HWIO_SSC_QTMR_V1_CNTP_CTL_0_ISTAT_SHFT                            0x2
#define HWIO_SSC_QTMR_V1_CNTP_CTL_0_IMSK_BMSK                             0x2
#define HWIO_SSC_QTMR_V1_CNTP_CTL_0_IMSK_SHFT                             0x1
#define HWIO_SSC_QTMR_V1_CNTP_CTL_0_EN_BMSK                               0x1
#define HWIO_SSC_QTMR_V1_CNTP_CTL_0_EN_SHFT                               0x0

#define HWIO_SSC_QTMR_V1_CNTV_CVAL_LO_0_ADDR                       (SSC_QTMR_F0_0_REG_BASE      + 0x00000030)
#define HWIO_SSC_QTMR_V1_CNTV_CVAL_LO_0_RMSK                       0xffffffff
#define HWIO_SSC_QTMR_V1_CNTV_CVAL_LO_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTV_CVAL_LO_0_ADDR, HWIO_SSC_QTMR_V1_CNTV_CVAL_LO_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTV_CVAL_LO_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTV_CVAL_LO_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTV_CVAL_LO_0_OUT(v)      \
        out_dword(HWIO_SSC_QTMR_V1_CNTV_CVAL_LO_0_ADDR,v)
#define HWIO_SSC_QTMR_V1_CNTV_CVAL_LO_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QTMR_V1_CNTV_CVAL_LO_0_ADDR,m,v,HWIO_SSC_QTMR_V1_CNTV_CVAL_LO_0_IN)
#define HWIO_SSC_QTMR_V1_CNTV_CVAL_LO_0_CNTV_CVAL_L0_BMSK          0xffffffff
#define HWIO_SSC_QTMR_V1_CNTV_CVAL_LO_0_CNTV_CVAL_L0_SHFT                 0x0

#define HWIO_SSC_QTMR_V1_CNTV_CVAL_HI_0_ADDR                       (SSC_QTMR_F0_0_REG_BASE      + 0x00000034)
#define HWIO_SSC_QTMR_V1_CNTV_CVAL_HI_0_RMSK                         0xffffff
#define HWIO_SSC_QTMR_V1_CNTV_CVAL_HI_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTV_CVAL_HI_0_ADDR, HWIO_SSC_QTMR_V1_CNTV_CVAL_HI_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTV_CVAL_HI_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTV_CVAL_HI_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTV_CVAL_HI_0_OUT(v)      \
        out_dword(HWIO_SSC_QTMR_V1_CNTV_CVAL_HI_0_ADDR,v)
#define HWIO_SSC_QTMR_V1_CNTV_CVAL_HI_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QTMR_V1_CNTV_CVAL_HI_0_ADDR,m,v,HWIO_SSC_QTMR_V1_CNTV_CVAL_HI_0_IN)
#define HWIO_SSC_QTMR_V1_CNTV_CVAL_HI_0_CNTV_CVAL_HI_BMSK            0xffffff
#define HWIO_SSC_QTMR_V1_CNTV_CVAL_HI_0_CNTV_CVAL_HI_SHFT                 0x0

#define HWIO_SSC_QTMR_V1_CNTV_TVAL_0_ADDR                          (SSC_QTMR_F0_0_REG_BASE      + 0x00000038)
#define HWIO_SSC_QTMR_V1_CNTV_TVAL_0_RMSK                          0xffffffff
#define HWIO_SSC_QTMR_V1_CNTV_TVAL_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTV_TVAL_0_ADDR, HWIO_SSC_QTMR_V1_CNTV_TVAL_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTV_TVAL_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTV_TVAL_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTV_TVAL_0_OUT(v)      \
        out_dword(HWIO_SSC_QTMR_V1_CNTV_TVAL_0_ADDR,v)
#define HWIO_SSC_QTMR_V1_CNTV_TVAL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QTMR_V1_CNTV_TVAL_0_ADDR,m,v,HWIO_SSC_QTMR_V1_CNTV_TVAL_0_IN)
#define HWIO_SSC_QTMR_V1_CNTV_TVAL_0_CNTV_TVAL_BMSK                0xffffffff
#define HWIO_SSC_QTMR_V1_CNTV_TVAL_0_CNTV_TVAL_SHFT                       0x0

#define HWIO_SSC_QTMR_V1_CNTV_CTL_0_ADDR                           (SSC_QTMR_F0_0_REG_BASE      + 0x0000003c)
#define HWIO_SSC_QTMR_V1_CNTV_CTL_0_RMSK                                  0x7
#define HWIO_SSC_QTMR_V1_CNTV_CTL_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTV_CTL_0_ADDR, HWIO_SSC_QTMR_V1_CNTV_CTL_0_RMSK)
#define HWIO_SSC_QTMR_V1_CNTV_CTL_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_CNTV_CTL_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_CNTV_CTL_0_OUT(v)      \
        out_dword(HWIO_SSC_QTMR_V1_CNTV_CTL_0_ADDR,v)
#define HWIO_SSC_QTMR_V1_CNTV_CTL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QTMR_V1_CNTV_CTL_0_ADDR,m,v,HWIO_SSC_QTMR_V1_CNTV_CTL_0_IN)
#define HWIO_SSC_QTMR_V1_CNTV_CTL_0_ISTAT_BMSK                            0x4
#define HWIO_SSC_QTMR_V1_CNTV_CTL_0_ISTAT_SHFT                            0x2
#define HWIO_SSC_QTMR_V1_CNTV_CTL_0_IMSK_BMSK                             0x2
#define HWIO_SSC_QTMR_V1_CNTV_CTL_0_IMSK_SHFT                             0x1
#define HWIO_SSC_QTMR_V1_CNTV_CTL_0_EN_BMSK                               0x1
#define HWIO_SSC_QTMR_V1_CNTV_CTL_0_EN_SHFT                               0x0

#define HWIO_SSC_QTMR_V1_VERSION_0_ADDR                            (SSC_QTMR_F0_0_REG_BASE      + 0x00000fd0)
#define HWIO_SSC_QTMR_V1_VERSION_0_RMSK                            0xffffffff
#define HWIO_SSC_QTMR_V1_VERSION_0_IN          \
        in_dword_masked(HWIO_SSC_QTMR_V1_VERSION_0_ADDR, HWIO_SSC_QTMR_V1_VERSION_0_RMSK)
#define HWIO_SSC_QTMR_V1_VERSION_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QTMR_V1_VERSION_0_ADDR, m)
#define HWIO_SSC_QTMR_V1_VERSION_0_MAJOR_BMSK                      0xf0000000
#define HWIO_SSC_QTMR_V1_VERSION_0_MAJOR_SHFT                            0x1c
#define HWIO_SSC_QTMR_V1_VERSION_0_MINOR_BMSK                       0xfff0000
#define HWIO_SSC_QTMR_V1_VERSION_0_MINOR_SHFT                            0x10
#define HWIO_SSC_QTMR_V1_VERSION_0_STEP_BMSK                           0xffff
#define HWIO_SSC_QTMR_V1_VERSION_0_STEP_SHFT                              0x0


#endif /* __HWSOLVER_HWIO_H__ */
