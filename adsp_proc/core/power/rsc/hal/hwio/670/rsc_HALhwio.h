#ifndef __RSC_HALHWIO_H__
#define __RSC_HALHWIO_H__
/*
===========================================================================
*/
/**
  @file rsc_HALhwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r103_MTO]
 
  This file contains HWIO register definitions for the following modules:
    LPASS_RSCC_RSCC_RSC
    TURING_RSCC_RSCC_RSC
    SSC_RSCC_RSCC_RSC
    LPASS_CC
    TURING_CC


  Generation parameters: 
  { u'filename': u'rsc_HALhwio.h',
    u'header': u'#include "msmhwiobase.h"',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [ u'LPASS_RSCC_RSCC_RSC',
                  u'TURING_RSCC_RSCC_RSC',
                  u'SSC_RSCC_RSCC_RSC',
                  u'LPASS_CC',
                  u'TURING_CC']}
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

  $Header: //components/rel/core.qdsp6/2.1/power/rsc/hal/hwio/670/rsc_HALhwio.h#1 $
  $DateTime: 2017/10/12 22:35:09 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: TURING_RSCC_RSCC_RSC
 *--------------------------------------------------------------------------*/

#define TURING_RSCC_RSCC_RSC_REG_BASE                                                                              (TURING_BASE      + 0x000a4000)

#define HWIO_TURING_RSCC_RSC_ID_DRVd_ADDR(d)                                                                       (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000000 + 0x10000 * (d))
#define HWIO_TURING_RSCC_RSC_ID_DRVd_RMSK                                                                            0xffffff
#define HWIO_TURING_RSCC_RSC_ID_DRVd_MAXd                                                                                   0
#define HWIO_TURING_RSCC_RSC_ID_DRVd_INI(d)        \
        in_dword_masked(HWIO_TURING_RSCC_RSC_ID_DRVd_ADDR(d), HWIO_TURING_RSCC_RSC_ID_DRVd_RMSK)
#define HWIO_TURING_RSCC_RSC_ID_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_RSC_ID_DRVd_ADDR(d), mask)
#define HWIO_TURING_RSCC_RSC_ID_DRVd_MAJOR_VER_BMSK                                                                  0xff0000
#define HWIO_TURING_RSCC_RSC_ID_DRVd_MAJOR_VER_SHFT                                                                      0x10
#define HWIO_TURING_RSCC_RSC_ID_DRVd_MINOR_VER_BMSK                                                                    0xff00
#define HWIO_TURING_RSCC_RSC_ID_DRVd_MINOR_VER_SHFT                                                                       0x8
#define HWIO_TURING_RSCC_RSC_ID_DRVd_STEP_VER_BMSK                                                                       0xff
#define HWIO_TURING_RSCC_RSC_ID_DRVd_STEP_VER_SHFT                                                                        0x0

#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(d)                                                      (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000004 + 0x10000 * (d))
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_RMSK                                                          0x71f1f1f
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_MAXd                                                                  0
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_INI(d)        \
        in_dword_masked(HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(d), HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_RMSK)
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(d), mask)
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SEQ_EXTENDED_BMSK                                             0x4000000
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SEQ_EXTENDED_SHFT                                                  0x1a
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_IS_CHILD_RSC_BMSK                                             0x2000000
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_IS_CHILD_RSC_SHFT                                                  0x19
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_HW_SOLVER_BMSK                                                0x1000000
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_HW_SOLVER_SHFT                                                     0x18
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_SLOTS_BMSK                                              0x1f0000
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_SLOTS_SHFT                                                  0x10
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_MODES_BMSK                                                0x1f00
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_MODES_SHFT                                                   0x8
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_NUM_TIMERS_BMSK                                                    0x1f
#define HWIO_TURING_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_NUM_TIMERS_SHFT                                                     0x0

#define HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(d)                                                         (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000008 + 0x10000 * (d))
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_RMSK                                                             0x7ff0f1f
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_MAXd                                                                     0
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_INI(d)        \
        in_dword_masked(HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(d), HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_RMSK)
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(d), mask)
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_DRV_BMSK                                                     0x7000000
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_DRV_SHFT                                                          0x18
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_SEQ_CMD_WORDS_BMSK                                            0xff0000
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_SEQ_CMD_WORDS_SHFT                                                0x10
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_TS_EVENTS_BMSK                                                   0xf00
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_TS_EVENTS_SHFT                                                     0x8
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_DELAY_CNTR_BITWIDTH_BMSK                                              0x1f
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_DELAY_CNTR_BITWIDTH_SHFT                                               0x0

#define HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(d)                                             (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x0000000c + 0x10000 * (d))
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_RMSK                                                0xf8ffffff
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_MAXd                                                         0
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_INI(d)        \
        in_dword_masked(HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(d), HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_RMSK)
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(d), mask)
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_CMDS_PER_TCS_BMSK                               0xf8000000
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_CMDS_PER_TCS_SHFT                                     0x1b
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV3_BMSK                                     0xfc0000
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV3_SHFT                                         0x12
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV2_BMSK                                      0x3f000
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV2_SHFT                                          0xc
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV1_BMSK                                        0xfc0
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV1_SHFT                                          0x6
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV0_BMSK                                         0x3f
#define HWIO_TURING_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV0_SHFT                                          0x0

#define HWIO_TURING_RSCC_RSC_STATUS0_DRVd_ADDR(d)                                                                  (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000010 + 0x10000 * (d))
#define HWIO_TURING_RSCC_RSC_STATUS0_DRVd_RMSK                                                                      0x101ffff
#define HWIO_TURING_RSCC_RSC_STATUS0_DRVd_MAXd                                                                              0
#define HWIO_TURING_RSCC_RSC_STATUS0_DRVd_INI(d)        \
        in_dword_masked(HWIO_TURING_RSCC_RSC_STATUS0_DRVd_ADDR(d), HWIO_TURING_RSCC_RSC_STATUS0_DRVd_RMSK)
#define HWIO_TURING_RSCC_RSC_STATUS0_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_RSC_STATUS0_DRVd_ADDR(d), mask)
#define HWIO_TURING_RSCC_RSC_STATUS0_DRVd_OUTI(d,val)    \
        out_dword(HWIO_TURING_RSCC_RSC_STATUS0_DRVd_ADDR(d),val)
#define HWIO_TURING_RSCC_RSC_STATUS0_DRVd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_STATUS0_DRVd_ADDR(d),mask,val,HWIO_TURING_RSCC_RSC_STATUS0_DRVd_INI(d))
#define HWIO_TURING_RSCC_RSC_STATUS0_DRVd_CLK_GATE_DISABLE_BMSK                                                     0x1000000
#define HWIO_TURING_RSCC_RSC_STATUS0_DRVd_CLK_GATE_DISABLE_SHFT                                                          0x18
#define HWIO_TURING_RSCC_RSC_STATUS0_DRVd_STATUS_SLEEP_ENTRY_DONE_BMSK                                                0x10000
#define HWIO_TURING_RSCC_RSC_STATUS0_DRVd_STATUS_SLEEP_ENTRY_DONE_SHFT                                                   0x10
#define HWIO_TURING_RSCC_RSC_STATUS0_DRVd_STATUS_SLEEP_CTL_FSM_BMSK                                                    0xffff
#define HWIO_TURING_RSCC_RSC_STATUS0_DRVd_STATUS_SLEEP_CTL_FSM_SHFT                                                       0x0

#define HWIO_TURING_RSCC_RSC_STATUS1_DRVd_ADDR(d)                                                                  (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000014 + 0x10000 * (d))
#define HWIO_TURING_RSCC_RSC_STATUS1_DRVd_RMSK                                                                     0xffffffff
#define HWIO_TURING_RSCC_RSC_STATUS1_DRVd_MAXd                                                                              0
#define HWIO_TURING_RSCC_RSC_STATUS1_DRVd_INI(d)        \
        in_dword_masked(HWIO_TURING_RSCC_RSC_STATUS1_DRVd_ADDR(d), HWIO_TURING_RSCC_RSC_STATUS1_DRVd_RMSK)
#define HWIO_TURING_RSCC_RSC_STATUS1_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_RSC_STATUS1_DRVd_ADDR(d), mask)
#define HWIO_TURING_RSCC_RSC_STATUS1_DRVd_PDC_MATCH_VALUE_LO_BMSK                                                  0xffffffff
#define HWIO_TURING_RSCC_RSC_STATUS1_DRVd_PDC_MATCH_VALUE_LO_SHFT                                                         0x0

#define HWIO_TURING_RSCC_RSC_STATUS2_DRVd_ADDR(d)                                                                  (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000018 + 0x10000 * (d))
#define HWIO_TURING_RSCC_RSC_STATUS2_DRVd_RMSK                                                                       0xffffff
#define HWIO_TURING_RSCC_RSC_STATUS2_DRVd_MAXd                                                                              0
#define HWIO_TURING_RSCC_RSC_STATUS2_DRVd_INI(d)        \
        in_dword_masked(HWIO_TURING_RSCC_RSC_STATUS2_DRVd_ADDR(d), HWIO_TURING_RSCC_RSC_STATUS2_DRVd_RMSK)
#define HWIO_TURING_RSCC_RSC_STATUS2_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_RSC_STATUS2_DRVd_ADDR(d), mask)
#define HWIO_TURING_RSCC_RSC_STATUS2_DRVd_PDC_MATCH_VALUE_HI_BMSK                                                    0xffffff
#define HWIO_TURING_RSCC_RSC_STATUS2_DRVd_PDC_MATCH_VALUE_HI_SHFT                                                         0x0

#define HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_ADDR                                                                 (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x0000001c)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_RMSK                                                                    0x10003
#define HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_ADDR, HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_RMSK)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_ADDR,m,v,HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_IN)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_TRIGGER_BMSK                                                 0x10000
#define HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_TRIGGER_SHFT                                                    0x10
#define HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_PDC_TIMER_DRV_OWNER_BMSK                                         0x3
#define HWIO_TURING_RSCC_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_PDC_TIMER_DRV_OWNER_SHFT                                         0x0

#define HWIO_TURING_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR                                                   (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000020)
#define HWIO_TURING_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_RMSK                                                       0xffff
#define HWIO_TURING_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR, HWIO_TURING_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_RMSK)
#define HWIO_TURING_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR,m,v,HWIO_TURING_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_IN)
#define HWIO_TURING_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_PDC_SEQ_START_ADDR_REG_OFFSET_BMSK                         0xffff
#define HWIO_TURING_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_PDC_SEQ_START_ADDR_REG_OFFSET_SHFT                            0x0

#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR                                                   (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000024)
#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_RMSK                                                       0xffff
#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR, HWIO_TURING_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_RMSK)
#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR,m,v,HWIO_TURING_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_IN)
#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_PDC_MATCH_VALUE_LO_REG_OFFSET_BMSK                         0xffff
#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_PDC_MATCH_VALUE_LO_REG_OFFSET_SHFT                            0x0

#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR                                                   (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000028)
#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_RMSK                                                       0xffff
#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR, HWIO_TURING_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_RMSK)
#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR,m,v,HWIO_TURING_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_IN)
#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_PDC_MATCH_VALUE_HI_REG_OFFSET_BMSK                         0xffff
#define HWIO_TURING_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_PDC_MATCH_VALUE_HI_REG_OFFSET_SHFT                            0x0

#define HWIO_TURING_RSCC_PDC_SLAVE_ID_DRV0_ADDR                                                                    (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x0000002c)
#define HWIO_TURING_RSCC_PDC_SLAVE_ID_DRV0_RMSK                                                                           0x7
#define HWIO_TURING_RSCC_PDC_SLAVE_ID_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_PDC_SLAVE_ID_DRV0_ADDR, HWIO_TURING_RSCC_PDC_SLAVE_ID_DRV0_RMSK)
#define HWIO_TURING_RSCC_PDC_SLAVE_ID_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_PDC_SLAVE_ID_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_PDC_SLAVE_ID_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_PDC_SLAVE_ID_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_PDC_SLAVE_ID_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_PDC_SLAVE_ID_DRV0_ADDR,m,v,HWIO_TURING_RSCC_PDC_SLAVE_ID_DRV0_IN)
#define HWIO_TURING_RSCC_PDC_SLAVE_ID_DRV0_PDC_SLAVE_ID_BMSK                                                              0x7
#define HWIO_TURING_RSCC_PDC_SLAVE_ID_DRV0_PDC_SLAVE_ID_SHFT                                                              0x0

#define HWIO_TURING_RSCC_HIDDEN_TCS_STATUS_DRVd_ADDR(d)                                                            (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000030 + 0x10000 * (d))
#define HWIO_TURING_RSCC_HIDDEN_TCS_STATUS_DRVd_RMSK                                                                  0x70707
#define HWIO_TURING_RSCC_HIDDEN_TCS_STATUS_DRVd_MAXd                                                                        0
#define HWIO_TURING_RSCC_HIDDEN_TCS_STATUS_DRVd_INI(d)        \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_STATUS_DRVd_ADDR(d), HWIO_TURING_RSCC_HIDDEN_TCS_STATUS_DRVd_RMSK)
#define HWIO_TURING_RSCC_HIDDEN_TCS_STATUS_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_STATUS_DRVd_ADDR(d), mask)
#define HWIO_TURING_RSCC_HIDDEN_TCS_STATUS_DRVd_COMPLETED_BMSK                                                        0x70000
#define HWIO_TURING_RSCC_HIDDEN_TCS_STATUS_DRVd_COMPLETED_SHFT                                                           0x10
#define HWIO_TURING_RSCC_HIDDEN_TCS_STATUS_DRVd_ISSUED_BMSK                                                             0x700
#define HWIO_TURING_RSCC_HIDDEN_TCS_STATUS_DRVd_ISSUED_SHFT                                                               0x8
#define HWIO_TURING_RSCC_HIDDEN_TCS_STATUS_DRVd_TRIGGERED_BMSK                                                            0x7
#define HWIO_TURING_RSCC_HIDDEN_TCS_STATUS_DRVd_TRIGGERED_SHFT                                                            0x0

#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR(d)                                                         (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000034 + 0x10000 * (d))
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_RMSK                                                            0xffffffff
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_MAXd                                                                     0
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_INI(d)        \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR(d), HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_RMSK)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR(d), mask)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR_BMSK                                                       0xffffffff
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR_SHFT                                                              0x0

#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(d)                                                         (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000038 + 0x10000 * (d))
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_RMSK                                                            0x80ffffff
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_MAXd                                                                     0
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_INI(d)        \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(d), HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_RMSK)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(d), mask)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_OUTI(d,val)    \
        out_dword(HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(d),val)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(d),mask,val,HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_INI(d))
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_VALID_BMSK                                      0x80000000
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_VALID_SHFT                                            0x1f
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_HI_BMSK                                           0xffffff
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_HI_SHFT                                                0x0

#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR(d)                                                         (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x0000003c + 0x10000 * (d))
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_RMSK                                                            0xffffffff
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_MAXd                                                                     0
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_INI(d)        \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR(d), HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_RMSK)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR(d), mask)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR_BMSK                                                       0xffffffff
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR_SHFT                                                              0x0

#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(d)                                                         (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000040 + 0x10000 * (d))
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_RMSK                                                            0xffffffff
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_MAXd                                                                     0
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_INI(d)        \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(d), HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_RMSK)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(d), mask)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_OUTI(d,val)    \
        out_dword(HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(d),val)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(d),mask,val,HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_INI(d))
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_PDC_MATCH_VALUE_LO_BMSK                                         0xffffffff
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_PDC_MATCH_VALUE_LO_SHFT                                                0x0

#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR(d)                                                         (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000044 + 0x10000 * (d))
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_RMSK                                                            0xffffffff
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_MAXd                                                                     0
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_INI(d)        \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR(d), HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_RMSK)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR(d), mask)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR_BMSK                                                       0xffffffff
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR_SHFT                                                              0x0

#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR                                                            (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000048)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_RMSK                                                            0x8000ffff
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR, HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_RMSK)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR,m,v,HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_IN)
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_VALID_BMSK                                   0x80000000
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_VALID_SHFT                                         0x1f
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_BMSK                                             0xffff
#define HWIO_TURING_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_SHFT                                                0x0

#define HWIO_TURING_RSCC_HW_EVENT_OWNER_DRV0_ADDR                                                                  (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x0000004c)
#define HWIO_TURING_RSCC_HW_EVENT_OWNER_DRV0_RMSK                                                                         0x3
#define HWIO_TURING_RSCC_HW_EVENT_OWNER_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_HW_EVENT_OWNER_DRV0_ADDR, HWIO_TURING_RSCC_HW_EVENT_OWNER_DRV0_RMSK)
#define HWIO_TURING_RSCC_HW_EVENT_OWNER_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_HW_EVENT_OWNER_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_HW_EVENT_OWNER_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_HW_EVENT_OWNER_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_HW_EVENT_OWNER_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_HW_EVENT_OWNER_DRV0_ADDR,m,v,HWIO_TURING_RSCC_HW_EVENT_OWNER_DRV0_IN)
#define HWIO_TURING_RSCC_HW_EVENT_OWNER_DRV0_OWNER_BMSK                                                                   0x3
#define HWIO_TURING_RSCC_HW_EVENT_OWNER_DRV0_OWNER_SHFT                                                                   0x0

#define HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(d,m)                                                       (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000050 + 0x4 * (m) + 0x10000 * (d))
#define HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_RMSK                                                            0x80000007
#define HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_MAXd                                                                     0
#define HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_MAXm                                                                    31
#define HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_INI2(d,m)        \
        in_dword_masked(HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(d,m), HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_RMSK)
#define HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(d,m), mask)
#define HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_OUTI2(d,m,val)    \
        out_dword(HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(d,m),val)
#define HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_OUTMI2(d,m,mask,val) \
        out_dword_masked_ns(HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(d,m),mask,val,HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_INI2(d,m))
#define HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ENABLE_BMSK                                                     0x80000000
#define HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ENABLE_SHFT                                                           0x1f
#define HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_EVENT_SELECT_BMSK                                                      0x7
#define HWIO_TURING_RSCC_HW_EVENT_MUXm_SELECT_DRVd_EVENT_SELECT_SHFT                                                      0x0

#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_ADDR                                                            (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x000000d0)
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_RMSK                                                                   0x1
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_ADDR, HWIO_TURING_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_EPCB_TIMEOUT_IRQ_BMSK                                                  0x1
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_EPCB_TIMEOUT_IRQ_SHFT                                                  0x0

#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_ADDR                                                             (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x000000d4)
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_RMSK                                                                    0x1
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_CLR_EPCB_TIMEOUT_IRQ_BMSK                                               0x1
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_CLR_EPCB_TIMEOUT_IRQ_SHFT                                               0x0

#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR                                                            (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x000000d8)
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_RMSK                                                              0x10ffff
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR, HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR,m,v,HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_IN)
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_IRQ_EN_BMSK                                          0x100000
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_IRQ_EN_SHFT                                              0x14
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_THRESHOLD_BMSK                                         0xffff
#define HWIO_TURING_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_THRESHOLD_SHFT                                            0x0

#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR                                                        (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000200)
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_RMSK                                                               0x3
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR, HWIO_TURING_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR,m,v,HWIO_TURING_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_IN)
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OWNER_BMSK                                                         0x3
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OWNER_SHFT                                                         0x0

#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(d,m)                                                     (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000204 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_RMSK                                                                 0x1
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_MAXd                                                                   0
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_MAXm                                                                   5
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_INI2(d,m)        \
        in_dword_masked(HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(d,m), HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_RMSK)
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(d,m), mask)
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_OUTI2(d,m,val)    \
        out_dword(HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(d,m),val)
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_OUTMI2(d,m,mask,val) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(d,m),mask,val,HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_INI2(d,m))
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ENABLE_BMSK                                                          0x1
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ENABLE_SHFT                                                          0x0

#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(d,m)                                            (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000208 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_RMSK                                                 0xffffffff
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_MAXd                                                          0
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_MAXm                                                          5
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_INI2(d,m)        \
        in_dword_masked(HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(d,m), HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_RMSK)
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(d,m), mask)
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_TS_DATA_L_BMSK                                       0xffffffff
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_TS_DATA_L_SHFT                                              0x0

#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(d,m)                                            (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x0000020c + 0x20 * (m) + 0x10000 * (d))
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_RMSK                                                   0xffffff
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_MAXd                                                          0
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_MAXm                                                          5
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_INI2(d,m)        \
        in_dword_masked(HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(d,m), HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_RMSK)
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(d,m), mask)
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_TS_DATA_H_BMSK                                         0xffffff
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_TS_DATA_H_SHFT                                              0x0

#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(d,m)                                                 (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000210 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_RMSK                                                           0x101
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_MAXd                                                               0
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_MAXm                                                               5
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_INI2(d,m)        \
        in_dword_masked(HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(d,m), HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_RMSK)
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(d,m), mask)
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_OVERFLOW_BMSK                                               0x100
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_OVERFLOW_SHFT                                                 0x8
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_VALID_BMSK                                                    0x1
#define HWIO_TURING_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_VALID_SHFT                                                    0x0

#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR                                                     (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000400)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_RMSK                                                     0x800003ff
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR, HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR,m,v,HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_IN)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_VALID_BMSK                                               0x80000000
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_VALID_SHFT                                                     0x1f
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR_BMSK                                                     0x3ff
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR_SHFT                                                       0x0

#define HWIO_TURING_RSCC_RSC_SEQ_BUSY_DRV0_ADDR                                                                    (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000404)
#define HWIO_TURING_RSCC_RSC_SEQ_BUSY_DRV0_RMSK                                                                           0x1
#define HWIO_TURING_RSCC_RSC_SEQ_BUSY_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_BUSY_DRV0_ADDR, HWIO_TURING_RSCC_RSC_SEQ_BUSY_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_SEQ_BUSY_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_BUSY_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_SEQ_BUSY_DRV0_BUSY_STAT_BMSK                                                                 0x1
#define HWIO_TURING_RSCC_RSC_SEQ_BUSY_DRV0_BUSY_STAT_SHFT                                                                 0x0

#define HWIO_TURING_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR                                                         (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000408)
#define HWIO_TURING_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_RMSK                                                              0x3ff
#define HWIO_TURING_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR, HWIO_TURING_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_CURRENT_INSTR_ADDR_BMSK                                           0x3ff
#define HWIO_TURING_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_CURRENT_INSTR_ADDR_SHFT                                             0x0

#define HWIO_TURING_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(i)                                                        (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000410 + 0x4 * (i))
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_RMSK                                                                0x3ff
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_MAXi                                                                    3
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INI(i)        \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(i), HWIO_TURING_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INMI(i,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(i), mask)
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_OUTI(i,val)    \
        out_dword(HWIO_TURING_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(i),val)
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_OUTMI(i,mask,val) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(i),mask,val,HWIO_TURING_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INI(i))
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR_BMSK                                                           0x3ff
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR_SHFT                                                             0x0

#define HWIO_TURING_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(i)                                                      (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000450 + 0x4 * (i))
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_RMSK                                                            0xfffff
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_MAXi                                                                  3
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INI(i)        \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(i), HWIO_TURING_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INMI(i,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(i), mask)
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_OUTI(i,val)    \
        out_dword(HWIO_TURING_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(i),val)
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_OUTMI(i,mask,val) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(i),mask,val,HWIO_TURING_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INI(i))
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_DELAY_VAL_BMSK                                                  0xfffff
#define HWIO_TURING_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_DELAY_VAL_SHFT                                                      0x0

#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR                                                        (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000460)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_RMSK                                                               0x1
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR, HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR,m,v,HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_IN)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_SEQ_OVERRIDE_TRIGGER_BMSK                                          0x1
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_SEQ_OVERRIDE_TRIGGER_SHFT                                          0x0

#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR                                          (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000464)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_RMSK                                               0x3ff
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR, HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR,m,v,HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_IN)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_SEQ_OVERRIDE_TRIGGER_START_ADDR_BMSK               0x3ff
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_SEQ_OVERRIDE_TRIGGER_START_ADDR_SHFT                 0x0

#define HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR                                                     (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000490)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_RMSK                                                     0x800003ff
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR, HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR,m,v,HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_IN)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_VALID_BMSK                                0x80000000
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_VALID_SHFT                                      0x1f
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_ADDR_BMSK                                      0x3ff
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_ADDR_SHFT                                        0x0

#define HWIO_TURING_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR                                                                (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000494)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_STEP_DRV0_RMSK                                                                       0x1
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_STEP_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR, HWIO_TURING_RSCC_RSC_SEQ_DBG_STEP_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_STEP_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_STEP_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_STEP_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR,m,v,HWIO_TURING_RSCC_RSC_SEQ_DBG_STEP_DRV0_IN)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_STEP_DRV0_DBG_STEP_TRIGGER_BMSK                                                      0x1
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_STEP_DRV0_DBG_STEP_TRIGGER_SHFT                                                      0x0

#define HWIO_TURING_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR                                                            (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000498)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_RMSK                                                                   0x1
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR, HWIO_TURING_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR,m,v,HWIO_TURING_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_IN)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_DBG_STEP_CONTINUE_BMSK                                                 0x1
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_DBG_STEP_CONTINUE_SHFT                                                 0x0

#define HWIO_TURING_RSCC_RSC_SEQ_DBG_STAT_DRV0_ADDR                                                                (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x0000049c)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_STAT_DRV0_RMSK                                                                       0x1
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_STAT_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_DBG_STAT_DRV0_ADDR, HWIO_TURING_RSCC_RSC_SEQ_DBG_STAT_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_STAT_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_DBG_STAT_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_STAT_DRV0_DBG_STEPPING_STATUS_BMSK                                                   0x1
#define HWIO_TURING_RSCC_RSC_SEQ_DBG_STAT_DRV0_DBG_STEPPING_STATUS_SHFT                                                   0x0

#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR                                                 (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x000004a0)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_RMSK                                                       0x3f
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR, HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR,m,v,HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_IN)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_SEQ_OVERRIDE_PWR_CNTRL_MASK_BMSK                           0x3f
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_SEQ_OVERRIDE_PWR_CNTRL_MASK_SHFT                            0x0

#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR                                                  (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x000004a4)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_RMSK                                                        0x3f
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR, HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR,m,v,HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_IN)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_SEQ_OVERRIDE_PWR_CNTRL_VAL_BMSK                             0x3f
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_SEQ_OVERRIDE_PWR_CNTRL_VAL_SHFT                              0x0

#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR                                                (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x000004a8)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_RMSK                                                      0x3f
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR, HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR,m,v,HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_IN)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_SEQ_OVERRIDE_WAIT_EVENT_MASK_BMSK                         0x3f
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_SEQ_OVERRIDE_WAIT_EVENT_MASK_SHFT                          0x0

#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR                                                 (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x000004ac)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_RMSK                                                       0x3f
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR, HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_RMSK)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR,m,v,HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_IN)
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_SEQ_OVERRIDE_WAIT_EVENT_VAL_BMSK                           0x3f
#define HWIO_TURING_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_SEQ_OVERRIDE_WAIT_EVENT_VAL_SHFT                            0x0

#define HWIO_TURING_RSCC_SEQ_MEM_m_DRV0_ADDR(m)                                                                    (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000600 + 0x4 * (m))
#define HWIO_TURING_RSCC_SEQ_MEM_m_DRV0_RMSK                                                                       0xffffffff
#define HWIO_TURING_RSCC_SEQ_MEM_m_DRV0_MAXm                                                                               23
#define HWIO_TURING_RSCC_SEQ_MEM_m_DRV0_INI(m)        \
        in_dword_masked(HWIO_TURING_RSCC_SEQ_MEM_m_DRV0_ADDR(m), HWIO_TURING_RSCC_SEQ_MEM_m_DRV0_RMSK)
#define HWIO_TURING_RSCC_SEQ_MEM_m_DRV0_INMI(m,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_SEQ_MEM_m_DRV0_ADDR(m), mask)
#define HWIO_TURING_RSCC_SEQ_MEM_m_DRV0_OUTI(m,val)    \
        out_dword(HWIO_TURING_RSCC_SEQ_MEM_m_DRV0_ADDR(m),val)
#define HWIO_TURING_RSCC_SEQ_MEM_m_DRV0_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TURING_RSCC_SEQ_MEM_m_DRV0_ADDR(m),mask,val,HWIO_TURING_RSCC_SEQ_MEM_m_DRV0_INI(m))
#define HWIO_TURING_RSCC_SEQ_MEM_m_DRV0_CMD_WORD_BMSK                                                              0xffffffff
#define HWIO_TURING_RSCC_SEQ_MEM_m_DRV0_CMD_WORD_SHFT                                                                     0x0

#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR                                                         (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000d00)
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_RMSK                                                                0xf
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR, HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_RMSK)
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR,m,v,HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_IN)
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_AMC_COMPLETION_IRQ_ENABLE_BMSK                                      0xf
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_AMC_COMPLETION_IRQ_ENABLE_SHFT                                      0x0

#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_ADDR                                                         (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000d04)
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_RMSK                                                                0xf
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_IN          \
        in_dword_masked(HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_ADDR, HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_RMSK)
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_INM(m)      \
        in_dword_masked(HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_ADDR, m)
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_AMC_COMPLETION_IRQ_STATUS_BMSK                                      0xf
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_AMC_COMPLETION_IRQ_STATUS_SHFT                                      0x0

#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_ADDR                                                          (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000d08)
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_RMSK                                                                 0xf
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_OUT(v)      \
        out_dword(HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_ADDR,v)
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_AMC_COMPLETION_IRQ_CLEAR_BMSK                                        0xf
#define HWIO_TURING_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_AMC_COMPLETION_IRQ_CLEAR_SHFT                                        0x0

#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(m)                                                       (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000d10 + 0x2A0 * (m))
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_RMSK                                                               0xfff
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_MAXm                                                                   3
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_INI(m)        \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(m), HWIO_TURING_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_RMSK)
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_INMI(m,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(m), mask)
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_OUTI(m,val)    \
        out_dword(HWIO_TURING_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(m),val)
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TURING_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(m),mask,val,HWIO_TURING_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_INI(m))
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_BMSK                                             0xfff
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_SHFT                                               0x0

#define HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_ADDR(m)                                                                 (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000d14 + 0x2A0 * (m))
#define HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_RMSK                                                                     0x1010000
#define HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_MAXm                                                                             3
#define HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_INI(m)        \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_ADDR(m), HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_RMSK)
#define HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_INMI(m,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_ADDR(m), mask)
#define HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_OUTI(m,val)    \
        out_dword(HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_ADDR(m),val)
#define HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_ADDR(m),mask,val,HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_INI(m))
#define HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_AMC_MODE_TRIGGER_BMSK                                                    0x1000000
#define HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_AMC_MODE_TRIGGER_SHFT                                                         0x18
#define HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_AMC_MODE_EN_BMSK                                                           0x10000
#define HWIO_TURING_RSCC_TCSm_DRV0_CONTROL_AMC_MODE_EN_SHFT                                                              0x10

#define HWIO_TURING_RSCC_TCSm_DRV0_STATUS_ADDR(m)                                                                  (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000d18 + 0x2A0 * (m))
#define HWIO_TURING_RSCC_TCSm_DRV0_STATUS_RMSK                                                                            0x1
#define HWIO_TURING_RSCC_TCSm_DRV0_STATUS_MAXm                                                                              3
#define HWIO_TURING_RSCC_TCSm_DRV0_STATUS_INI(m)        \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_DRV0_STATUS_ADDR(m), HWIO_TURING_RSCC_TCSm_DRV0_STATUS_RMSK)
#define HWIO_TURING_RSCC_TCSm_DRV0_STATUS_INMI(m,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_DRV0_STATUS_ADDR(m), mask)
#define HWIO_TURING_RSCC_TCSm_DRV0_STATUS_CONTROLLER_IDLE_BMSK                                                            0x1
#define HWIO_TURING_RSCC_TCSm_DRV0_STATUS_CONTROLLER_IDLE_SHFT                                                            0x0

#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_ENABLE_ADDR(m)                                                              (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000d1c + 0x2A0 * (m))
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_ENABLE_RMSK                                                                      0xfff
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_ENABLE_MAXm                                                                          3
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_ENABLE_INI(m)        \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_DRV0_CMD_ENABLE_ADDR(m), HWIO_TURING_RSCC_TCSm_DRV0_CMD_ENABLE_RMSK)
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_ENABLE_INMI(m,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_DRV0_CMD_ENABLE_ADDR(m), mask)
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_ENABLE_OUTI(m,val)    \
        out_dword(HWIO_TURING_RSCC_TCSm_DRV0_CMD_ENABLE_ADDR(m),val)
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_ENABLE_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TURING_RSCC_TCSm_DRV0_CMD_ENABLE_ADDR(m),mask,val,HWIO_TURING_RSCC_TCSm_DRV0_CMD_ENABLE_INI(m))
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_ENABLE_INDIVIDUAL_CMD_ENABLE_MASK_BMSK                                           0xfff
#define HWIO_TURING_RSCC_TCSm_DRV0_CMD_ENABLE_INDIVIDUAL_CMD_ENABLE_MASK_SHFT                                             0x0

#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_ADDR(m,n)                                                            (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000d30 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_RMSK                                                                    0x1010f
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_MAXm                                                                          3
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_MAXn                                                                         11
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_INI2(m,n)        \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_ADDR(m,n), HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_RMSK)
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_ADDR(m,n), mask)
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_OUTI2(m,n,val)    \
        out_dword(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_ADDR(m,n),val)
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_OUTMI2(m,n,mask,val) \
        out_dword_masked_ns(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_ADDR(m,n),mask,val,HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_INI2(m,n))
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_READ_OR_WRITE_BMSK                                                      0x10000
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_READ_OR_WRITE_SHFT                                                         0x10
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_RES_REQ_BMSK                                                              0x100
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_RES_REQ_SHFT                                                                0x8
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_MSG_LENGTH_BMSK                                                             0xf
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_MSGID_MSG_LENGTH_SHFT                                                             0x0

#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_ADDR(m,n)                                                             (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000d34 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_RMSK                                                                     0x7ffff
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_MAXm                                                                           3
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_MAXn                                                                          11
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_INI2(m,n)        \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_ADDR(m,n), HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_RMSK)
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_ADDR(m,n), mask)
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_OUTI2(m,n,val)    \
        out_dword(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_ADDR(m,n),val)
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_OUTMI2(m,n,mask,val) \
        out_dword_masked_ns(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_ADDR(m,n),mask,val,HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_INI2(m,n))
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_SLV_ID_BMSK                                                              0x70000
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_SLV_ID_SHFT                                                                 0x10
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_OFFSET_BMSK                                                               0xffff
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_ADDR_OFFSET_SHFT                                                                  0x0

#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_ADDR(m,n)                                                             (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000d38 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_RMSK                                                                  0xffffffff
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_MAXm                                                                           3
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_MAXn                                                                          11
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_INI2(m,n)        \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_ADDR(m,n), HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_RMSK)
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_ADDR(m,n), mask)
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_OUTI2(m,n,val)    \
        out_dword(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_ADDR(m,n),val)
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_OUTMI2(m,n,mask,val) \
        out_dword_masked_ns(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_ADDR(m,n),mask,val,HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_INI2(m,n))
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_DATA_BMSK                                                             0xffffffff
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_DATA_DATA_SHFT                                                                    0x0

#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_STATUS_ADDR(m,n)                                                           (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000d3c + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_STATUS_RMSK                                                                   0x10101
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_STATUS_MAXm                                                                         3
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_STATUS_MAXn                                                                        11
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_STATUS_INI2(m,n)        \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_STATUS_ADDR(m,n), HWIO_TURING_RSCC_TCSm_CMDn_DRV0_STATUS_RMSK)
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_STATUS_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_STATUS_ADDR(m,n), mask)
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_STATUS_COMPLETED_BMSK                                                         0x10000
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_STATUS_COMPLETED_SHFT                                                            0x10
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_STATUS_ISSUED_BMSK                                                              0x100
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_STATUS_ISSUED_SHFT                                                                0x8
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_STATUS_TRIGGERED_BMSK                                                             0x1
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_STATUS_TRIGGERED_SHFT                                                             0x0

#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_ADDR(m,n)                                               (TURING_RSCC_RSCC_RSC_REG_BASE      + 0x00000d40 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_RMSK                                                    0xffffffff
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_MAXm                                                             3
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_MAXn                                                            11
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_INI2(m,n)        \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_ADDR(m,n), HWIO_TURING_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_RMSK)
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_TURING_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_ADDR(m,n), mask)
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_READ_RESPONSE_DATA_BMSK                                 0xffffffff
#define HWIO_TURING_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_READ_RESPONSE_DATA_SHFT                                        0x0

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
 * MODULE: LPASS_CC
 *--------------------------------------------------------------------------*/

#define LPASS_CC_REG_BASE                                                                                    (LPASS_BASE      + 0x00c00000)

#define HWIO_LPASS_LPAAUDIO_PLL_MODE_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x00000000)
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_RMSK                                                                    0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_MODE_ADDR, HWIO_LPASS_LPAAUDIO_PLL_MODE_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_MODE_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_MODE_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_MODE_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_MODE_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_LOCK_DET_BMSK                                                       0x80000000
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_LOCK_DET_SHFT                                                             0x1f
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                                                    0x40000000
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                                                          0x1e
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_RESERVE_BITS29_22_BMSK                                                  0x3fc00000
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_RESERVE_BITS29_22_SHFT                                                        0x16
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                                                   0x200000
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                                                       0x15
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                                                     0x100000
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                                                         0x14
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_BIAS_COUNT_BMSK                                                        0xfc000
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_BIAS_COUNT_SHFT                                                            0xe
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_LOCK_COUNT_BMSK                                                         0x3f00
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_LOCK_COUNT_SHFT                                                            0x8
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_RESERVE_BITS7_4_BMSK                                                          0xf0
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_RESERVE_BITS7_4_SHFT                                                           0x4
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_PLLTEST_BMSK                                                               0x8
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_PLLTEST_SHFT                                                               0x3
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_RESET_N_BMSK                                                               0x4
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_RESET_N_SHFT                                                               0x2
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_BYPASSNL_BMSK                                                              0x2
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_BYPASSNL_SHFT                                                              0x1
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_OUTCTRL_BMSK                                                               0x1
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_OUTCTRL_SHFT                                                               0x0

#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00000004)
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_RMSK                                                                   0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_L_VAL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_L_VAL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_L_VAL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_L_VAL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_L_VAL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_L_VAL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_RESERVE_BITS_31_8_BMSK                                                 0xffffff00
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_RESERVE_BITS_31_8_SHFT                                                        0x8
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_PLL_L_BMSK                                                                   0xff
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_PLL_L_SHFT                                                                    0x0

#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00000008)
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_RMSK                                                               0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_RESERVE_BITS_31_16_BMSK                                            0xffff0000
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_RESERVE_BITS_31_16_SHFT                                                  0x10
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_PLL_ALPHA_BMSK                                                         0xffff
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_PLL_ALPHA_SHFT                                                            0x0

#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADDR                                                                (LPASS_CC_REG_BASE      + 0x0000000c)
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RMSK                                                                0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BITS31_28_BMSK                                              0xf0000000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BITS31_28_SHFT                                                    0x1c
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_SSC_EN_BMSK                                                          0x8000000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_SSC_EN_SHFT                                                               0x1b
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BIT26_BMSK                                                   0x4000000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BIT26_SHFT                                                        0x1a
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ALPHA_MODE_BMSK                                                      0x2000000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ALPHA_MODE_SHFT                                                           0x19
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ALPHA_EN_BMSK                                                        0x1000000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ALPHA_EN_SHFT                                                             0x18
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DCO_FREQ_STEP_BMSK                                                    0x800000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DCO_FREQ_STEP_SHFT                                                        0x17
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADC_CONV_TIME_BUDGET_BMSK                                             0x400000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADC_CONV_TIME_BUDGET_SHFT                                                 0x16
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DCO_DATA_LATCH_EDGE_BMSK                                              0x200000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DCO_DATA_LATCH_EDGE_SHFT                                                  0x15
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DCO_POST_DIV_EN_BMSK                                                  0x100000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DCO_POST_DIV_EN_SHFT                                                      0x14
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BIT19_BMSK                                                     0x80000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BIT19_SHFT                                                        0x13
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_L_VAL_CHANGE_TM_EN_BMSK                                                0x40000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_L_VAL_CHANGE_TM_EN_SHFT                                                   0x12
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_OLC_EXIT_COUNT_BMSK                                                    0x20000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_OLC_EXIT_COUNT_SHFT                                                       0x11
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADC_STATE_SYNC_DIS_BMSK                                                0x10000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADC_STATE_SYNC_DIS_SHFT                                                   0x10
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADC_LOCK_SYNC_DIS_BMSK                                                  0x8000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADC_LOCK_SYNC_DIS_SHFT                                                     0xf
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DROOP_BYPASS_CFA_FFA_BMSK                                               0x4000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DROOP_BYPASS_CFA_FFA_SHFT                                                  0xe
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_CFA_MIN_TIME_DIS_BMSK                                                   0x2000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_CFA_MIN_TIME_DIS_SHFT                                                      0xd
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PRE_DIV2_EN_BMSK                                                        0x1000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PRE_DIV2_EN_SHFT                                                           0xc
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DROOP_DIV_BIT6_BMSK                                                      0x800
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DROOP_DIV_BIT6_SHFT                                                        0xb
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DROOP_DIV_2_3_BMSK                                                       0x400
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DROOP_DIV_2_3_SHFT                                                         0xa
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_POSTDIV_CTL_BMSK                                                         0x300
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_POSTDIV_CTL_SHFT                                                           0x8
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_OUT_CLK_POLARITY_BMSK                                                     0x80
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_OUT_CLK_POLARITY_SHFT                                                      0x7
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ANALOG_DROOP_ENABLE_BMSK                                                  0x40
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ANALOG_DROOP_ENABLE_SHFT                                                   0x6
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BIT_5_BMSK                                                        0x20
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BIT_5_SHFT                                                         0x5
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_TEST_BMSK                                                       0x10
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_TEST_SHFT                                                        0x4
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_EARLY_BMSK                                                       0x8
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_EARLY_SHFT                                                       0x3
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_AUX2_BMSK                                                        0x4
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_AUX2_SHFT                                                        0x2
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_AUX_BMSK                                                         0x2
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_AUX_SHFT                                                         0x1
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK                                                        0x1
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_MAIN_SHFT                                                        0x0

#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_ADDR                                                              (LPASS_CC_REG_BASE      + 0x00000010)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_RMSK                                                              0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_SAR_COMM_VOL_ADJ_BMSK                                             0xc0000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_SAR_COMM_VOL_ADJ_SHFT                                                   0x1e
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_SAR_REF_BMSK                                                      0x30000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_SAR_REF_SHFT                                                            0x1c
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_SAR_DEL_BMSK                                                       0x8000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_SAR_DEL_SHFT                                                            0x1b
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_CHP_BS_DEL_BMSK                                                    0x4000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_CHP_BS_DEL_SHFT                                                         0x1a
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_REF_TRIM_BMSK                                                      0x3800000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_REF_TRIM_SHFT                                                           0x17
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_DCO_ADD_EN_BMSK                                                     0x400000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_DCO_ADD_EN_SHFT                                                         0x16
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_FBC_ALPHA_CAL_SEL_BMSK                                              0x300000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_FBC_ALPHA_CAL_SEL_SHFT                                                  0x14
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_TDL_CONTROL_BMSK                                                     0xfc000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_TDL_CONTROL_SHFT                                                         0xe
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_PFA_MSB_BMSK                                                          0x3c00
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_PFA_MSB_SHFT                                                             0xa
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_P_TM_SEL_BMSK                                                        0x300
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_P_TM_SEL_SHFT                                                          0x8
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_I_TM_SEL_BMSK                                                         0xc0
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_I_TM_SEL_SHFT                                                          0x6
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_P_FFA_SEL_BMSK                                                        0x30
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_P_FFA_SEL_SHFT                                                         0x4
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_I_FFA_SEL_BMSK                                                         0xc
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_I_FFA_SEL_SHFT                                                         0x2
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_PFD_DZSEL_BMSK                                                           0x3
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_PFD_DZSEL_SHFT                                                           0x0

#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_ADDR                                                            (LPASS_CC_REG_BASE      + 0x00000014)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_RMSK                                                            0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_ADDR, HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_FORCE_CFA_MODE_BMSK                                             0x80000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_FORCE_CFA_MODE_SHFT                                                   0x1f
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_BYPASS_DIS_BMSK                                             0x40000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_BYPASS_DIS_SHFT                                                   0x1e
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DROOP_EN_EDGE_ANA_EN_BMSK                                       0x20000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DROOP_EN_EDGE_ANA_EN_SHFT                                             0x1d
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DROOP_DIV_DIS_BMSK                                              0x10000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DROOP_DIV_DIS_SHFT                                                    0x1c
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_CAL_DIV4_BMSK                                                0x8000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_CAL_DIV4_SHFT                                                     0x1b
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_CONFIG_CTL_BMSK                                              0x7000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_CONFIG_CTL_SHFT                                                   0x18
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_EN_MAIN_AUX_BMSK                                              0x800000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_EN_MAIN_AUX_SHFT                                                  0x17
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TIE_CORRECTION_SETTING_BMSK                                       0x780000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TIE_CORRECTION_SETTING_SHFT                                           0x13
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DROOP_DIV_CONT_BMSK                                                0x7e000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DROOP_DIV_CONT_SHFT                                                    0xd
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DUTY_CYC_CTL_OVRD_BMSK                                              0x1000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DUTY_CYC_CTL_OVRD_SHFT                                                 0xc
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_CFA_GLITCH_DET_BYP_BMSK                                              0x800
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_CFA_GLITCH_DET_BYP_SHFT                                                0xb
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DUTY_CYC_ADJ_BMSK                                                    0x7c0
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DUTY_CYC_ADJ_SHFT                                                      0x6
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_OUTP_DUTY_CYC_CTL_BMSK                                                0x20
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_OUTP_DUTY_CYC_CTL_SHFT                                                 0x5
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_COUNTER_EN_DMET_BMSK                                                  0x10
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_COUNTER_EN_DMET_SHFT                                                   0x4
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_COUNTER_EN_SINGLE_DMET_BMSK                                            0x8
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_COUNTER_EN_SINGLE_DMET_SHFT                                            0x3
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_LOW_JIT_MODE_EN_BMSK                                                   0x4
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_LOW_JIT_MODE_EN_SHFT                                                   0x2
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_CHR_PUMP_REF_CUR_ADJ_BMSK                                              0x3
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_CHR_PUMP_REF_CUR_ADJ_SHFT                                              0x0

#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ADDR                                                                (LPASS_CC_REG_BASE      + 0x00000018)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_RMSK                                                                0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_DFINE_OVER_VAL_BMSK                                                 0xe0000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_DFINE_OVER_VAL_SHFT                                                       0x1d
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_LOCK_THR_DFINE_OVR_BMSK                                             0x18000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_LOCK_THR_DFINE_OVR_SHFT                                                   0x1b
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_LOCK_TIM_DFINE_OVR_BMSK                                              0x4000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_LOCK_TIM_DFINE_OVR_SHFT                                                   0x1a
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_D_FINE_OVERRIDE_BMSK                                                 0x2000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_D_FINE_OVERRIDE_SHFT                                                      0x19
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_G_BWC_OVERRIDE_VALUE_BMSK                                            0x1fc0000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_G_BWC_OVERRIDE_VALUE_SHFT                                                 0x12
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_G_BWC_OVERRIDE_BMSK                                                    0x20000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_G_BWC_OVERRIDE_SHFT                                                       0x11
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_STAY_IN_CFA_BMSK                                                       0x10000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_STAY_IN_CFA_SHFT                                                          0x10
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_SKIP_FFA_BMSK                                                           0x8000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_SKIP_FFA_SHFT                                                              0xf
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_SKIP_CFA_BMSK                                                           0x4000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_SKIP_CFA_SHFT                                                              0xe
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_RESET_FFA_TM_ACCUM_BMSK                                                 0x2000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_RESET_FFA_TM_ACCUM_SHFT                                                    0xd
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_RESET_CFA_ACCUM_BMSK                                                    0x1000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_RESET_CFA_ACCUM_SHFT                                                       0xc
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_INP_REF_CLK_SEL_BMSK                                                     0x800
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_INP_REF_CLK_SEL_SHFT                                                       0xb
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_DISABLE_LFSR_BMSK                                                        0x400
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_DISABLE_LFSR_SHFT                                                          0xa
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_PLL_STATUS_SEL_BMSK                                                      0x3c0
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_PLL_STATUS_SEL_SHFT                                                        0x6
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_DTEST_SEL_BMSK                                                            0x20
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_DTEST_SEL_SHFT                                                             0x5
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST_AMP_BYPASS_BMSK                                                     0x10
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST_AMP_BYPASS_SHFT                                                      0x4
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST_NMO_SEL_BMSK                                                         0xc
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST_NMO_SEL_SHFT                                                         0x2
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST1_EN_BMSK                                                             0x2
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST1_EN_SHFT                                                             0x1
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST0_EN_BMSK                                                             0x1
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST0_EN_SHFT                                                             0x0

#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ADDR                                                              (LPASS_CC_REG_BASE      + 0x0000001c)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_RMSK                                                              0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ADDR, HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_LV_TEST_SEL_BMSK                                                  0x80000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_LV_TEST_SEL_SHFT                                                        0x1f
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_CHRG_PUMP_PWR_DN_BMSK                                             0x40000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_CHRG_PUMP_PWR_DN_SHFT                                                   0x1e
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ENABLE_FBC_BMSK                                                   0x20000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ENABLE_FBC_SHFT                                                         0x1d
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_PTUNE_OVERRIDE_VAL_BMSK                                           0x1c000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_PTUNE_OVERRIDE_VAL_SHFT                                                 0x1a
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_PTUNE_OVERRIDE_BMSK                                                0x2000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_PTUNE_OVERRIDE_SHFT                                                     0x19
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_DIV_DC_SEL_BMSK                                                    0x1000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_DIV_DC_SEL_SHFT                                                         0x18
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_NGEN_CFG_BMSK                                                       0xc00000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_NGEN_CFG_SHFT                                                           0x16
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_NGEN_EN_BMSK                                                        0x200000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_NGEN_EN_SHFT                                                            0x15
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_CHRG_PUMP_BIAS_BYP_BMSK                                             0x100000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_CHRG_PUMP_BIAS_BYP_SHFT                                                 0x14
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_DEM_DCO_DAC_EN_BMSK                                                  0x80000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_DEM_DCO_DAC_EN_SHFT                                                     0x13
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_NMOSC_EN_BMSK                                                        0x40000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_NMOSC_EN_SHFT                                                           0x12
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_RESERVE_BIT49_BMSK                                                   0x20000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_RESERVE_BIT49_SHFT                                                      0x11
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_STATUS_OUT_TRAIN_SEQ_BMSK                                            0x10000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_STATUS_OUT_TRAIN_SEQ_SHFT                                               0x10
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ADC_TEST_CLK_SEL_BMSK                                                 0x8000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ADC_TEST_CLK_SEL_SHFT                                                    0xf
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_CTUNE_ADD_GLITCH_FILT_BMSK                                            0x4000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_CTUNE_ADD_GLITCH_FILT_SHFT                                               0xe
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_D_EXT_SEL_BMSK                                                        0x2000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_D_EXT_SEL_SHFT                                                           0xd
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_D_EXT_VAL_BMSK                                                        0x1ff0
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_D_EXT_VAL_SHFT                                                           0x4
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ENTER_OLC_MODE_SEL_BMSK                                                  0x8
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ENTER_OLC_MODE_SEL_SHFT                                                  0x3
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_DFINE_OVER_VAL_BMSK                                                      0x7
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_DFINE_OVER_VAL_SHFT                                                      0x0

#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_ADDR                                                         (LPASS_CC_REG_BASE      + 0x00000020)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_RMSK                                                         0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_ADDR, HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_RESERVE_BITS_31_8_BMSK                                       0xffffff00
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_RESERVE_BITS_31_8_SHFT                                              0x8
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_PLL_SCC_DELTA_ALPHA_BMSK                                           0xff
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_PLL_SCC_DELTA_ALPHA_SHFT                                            0x0

#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_ADDR                                                         (LPASS_CC_REG_BASE      + 0x00000024)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_RMSK                                                         0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_ADDR, HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_RESERVE_BITS_31_8_BMSK                                       0xffffff00
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_RESERVE_BITS_31_8_SHFT                                              0x8
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_PLL_SCC_UPDATE_RATE_BMSK                                           0xff
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_PLL_SCC_UPDATE_RATE_SHFT                                            0x0

#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00000028)
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_RMSK                                                               0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_RESERVE_BITS_31_15_BMSK                                            0xffff8000
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_RESERVE_BITS_31_15_SHFT                                                   0xf
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_DROOP_CODE_BMSK                                                        0x7f00
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_DROOP_CODE_SHFT                                                           0x8
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_RESERVE_BITS_7_1_BMSK                                                    0xfe
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_RESERVE_BITS_7_1_SHFT                                                     0x1
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_DROOP_EN_BMSK                                                             0x1
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_DROOP_EN_SHFT                                                             0x0

#define HWIO_LPASS_LPAAUDIO_PLL_STATUS_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x0000002c)
#define HWIO_LPASS_LPAAUDIO_PLL_STATUS_RMSK                                                                  0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_STATUS_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_STATUS_ADDR, HWIO_LPASS_LPAAUDIO_PLL_STATUS_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_STATUS_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_STATUS_STATUS_31_0_BMSK                                                      0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_STATUS_STATUS_31_0_SHFT                                                             0x0

#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_ADDR                                                                (LPASS_CC_REG_BASE      + 0x00000030)
#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_RMSK                                                                0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_BMSK                                             0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_SHFT                                                    0x0

#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_ADDR                                                                (LPASS_CC_REG_BASE      + 0x00000034)
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_RMSK                                                                0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_RESERVE_BITS_31_12_BMSK                                             0xfffff000
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_RESERVE_BITS_31_12_SHFT                                                    0xc
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_BIST_CTL_BMSK                                                            0xfff
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_BIST_CTL_SHFT                                                              0x0

#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_ADDR                                                              (LPASS_CC_REG_BASE      + 0x00000038)
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_RMSK                                                                    0x1f
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_ADDR, HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_AUX2_CGC_EN_BMSK                                                    0x10
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_AUX2_CGC_EN_SHFT                                                     0x4
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_AUX_SVS_CGC_EN_BMSK                                                  0x8
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_AUX_SVS_CGC_EN_SHFT                                                  0x3
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_AUX_CGC_EN_BMSK                                                      0x4
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_AUX_CGC_EN_SHFT                                                      0x2
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_MAIN_SVS_CGC_EN_BMSK                                                 0x2
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_MAIN_SVS_CGC_EN_SHFT                                                 0x1
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_MAIN_CGC_EN_BMSK                                                     0x1
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_MAIN_CGC_EN_SHFT                                                     0x0

#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x0000003c)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_RMSK                                                             0x3
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_IN          \
        in_dword_masked(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_ADDR, HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_RMSK)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_ADDR, m)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_OUT(v)      \
        out_dword(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_ADDR,v)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_ADDR,m,v,HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_IN)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_CLK_DIV_BMSK                                                     0x3
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_CLK_DIV_SHFT                                                     0x0

#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_ADDR                                                       (LPASS_CC_REG_BASE      + 0x00000040)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_RMSK                                                              0xf
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_IN          \
        in_dword_masked(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_ADDR, HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_RMSK)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_ADDR, m)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_OUT(v)      \
        out_dword(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_ADDR,v)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_ADDR,m,v,HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_IN)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_CLK_DIV_BMSK                                                      0xf
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_CLK_DIV_SHFT                                                      0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00001004)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_RMSK                                                                   0xffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_PLL_L_BMSK                                                             0xffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_PLL_L_SHFT                                                                0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_ADDR                                                           (LPASS_CC_REG_BASE      + 0x00001008)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_RMSK                                                               0xffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_PLL_CAL_L_BMSK                                                     0xffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_PLL_CAL_L_SHFT                                                        0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_ADDR                                                            (LPASS_CC_REG_BASE      + 0x0000100c)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RMSK                                                            0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RESERVE_BITS31_19_BMSK                                          0xfff80000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RESERVE_BITS31_19_SHFT                                                0x13
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                                                 0x70000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                                                    0x10
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_POST_DIV_RATIO_ODD_BMSK                                             0xf000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_POST_DIV_RATIO_ODD_SHFT                                                0xc
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_POST_DIV_RATIO_EVEN_BMSK                                             0xf00
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_POST_DIV_RATIO_EVEN_SHFT                                               0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_OUT_CLK_POLARITY_BMSK                                                 0x80
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_OUT_CLK_POLARITY_SHFT                                                  0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RESERVE_BITS6_5_BMSK                                                  0x60
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RESERVE_BITS6_5_SHFT                                                   0x5
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_TEST_BMSK                                                      0x10
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_TEST_SHFT                                                       0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RESERVE_BITS3_BMSK                                                     0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RESERVE_BITS3_SHFT                                                     0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_ODD_BMSK                                                        0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_ODD_SHFT                                                        0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_EVEN_BMSK                                                       0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_EVEN_SHFT                                                       0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_MAIN_BMSK                                                       0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_MAIN_SHFT                                                       0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_ADDR                                                          (LPASS_CC_REG_BASE      + 0x00001010)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_RMSK                                                          0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_RESERVE_BITS31_22_BMSK                                        0xffc00000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_RESERVE_BITS31_22_SHFT                                              0x16
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_FREEZE_LOCK_DETECTOR_IN_DROOP_BMSK                              0x200000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_FREEZE_LOCK_DETECTOR_IN_DROOP_SHFT                                  0x15
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_DROOP_DIVIDER_SEL_BMSK                                          0x100000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_DROOP_DIVIDER_SEL_SHFT                                              0x14
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_SCALE_FREQ_ON_DROOP_BMSK                                         0x80000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_SCALE_FREQ_ON_DROOP_SHFT                                            0x13
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_BMSK                                         0x40000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_SHFT                                            0x12
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_BMSK                                         0x20000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_SHFT                                            0x11
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_LOW_JITTER_MODE_EN_BMSK                                          0x10000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_LOW_JITTER_MODE_EN_SHFT                                             0x10
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_FRAC_FORMAT_SEL_BMSK                                              0x8000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_FRAC_FORMAT_SEL_SHFT                                                 0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_BIAS_ON_IN_STANDBY_BMSK                                           0x4000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_BIAS_ON_IN_STANDBY_SHFT                                              0xe
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_REF_CLK_AT_OUT_BMSK                                               0x2000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_REF_CLK_AT_OUT_SHFT                                                  0xd
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_SCALE_FREQ_ON_RESTART_BMSK                                        0x1800
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_SCALE_FREQ_ON_RESTART_SHFT                                           0xb
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_BMSK                                        0x400
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_SHFT                                          0xa
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                                               0x3e0
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                                                 0x5
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_WRITE_STATE_EN_BMSK                                                 0x10
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_WRITE_STATE_EN_SHFT                                                  0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_CALIB_CTRL_BMSK                                                      0xe
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_CALIB_CTRL_SHFT                                                      0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_BMSK                                          0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_SHFT                                          0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_ADDR                                                          (LPASS_CC_REG_BASE      + 0x00001014)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_RMSK                                                          0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FINE_LDC_THRESHOLD_BMSK                                       0xfc000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FINE_LDC_THRESHOLD_SHFT                                             0x1a
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_COARSE_LDC_THRESHOLD_BMSK                                      0x3c00000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_COARSE_LDC_THRESHOLD_SHFT                                           0x16
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_BMSK                              0x3c0000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_SHFT                                  0x12
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_BMSK                               0x3c000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_SHFT                                   0xe
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_BMSK                                      0x3800
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_SHFT                                         0xb
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_BMSK                                     0x700
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_SHFT                                       0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_BMSK                                           0xf0
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_SHFT                                            0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FWD_GAIN_KFN_BMSK                                                    0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FWD_GAIN_KFN_SHFT                                                    0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00001018)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_RMSK                                                        0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_BIST_CFG_BMSK                                               0xfff00000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_BIST_CFG_SHFT                                                     0x14
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_RESERVE_BITS_19_18_BMSK                                        0xc0000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_RESERVE_BITS_19_18_SHFT                                           0x12
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_FORCE_DAC_ON_BMSK                                              0x30000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_FORCE_DAC_ON_SHFT                                                 0x10
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_DIS_DMET_DROOP_DIV_BMSK                                         0x8000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_DIS_DMET_DROOP_DIV_SHFT                                            0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_BYPASS_DROOP_DIV_BMSK                                           0x4000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_BYPASS_DROOP_DIV_SHFT                                              0xe
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_OSC_SETTLING_TIME_BMSK                                          0x3000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_OSC_SETTLING_TIME_SHFT                                             0xc
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_BMSK                                         0xc00
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_SHFT                                           0xa
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_PREDIV_WHEN_CAL_BMSK                                             0x380
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_PREDIV_WHEN_CAL_SHFT                                               0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_BMSK                                  0x40
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_SHFT                                   0x6
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_BMSK                                         0x30
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_SHFT                                          0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_BMSK                                         0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_SHFT                                         0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_BIAS_WARMUP_TIME_BMSK                                              0x6
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_BIAS_WARMUP_TIME_SHFT                                              0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_BMSK                                       0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_SHFT                                       0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ADDR                                                            (LPASS_CC_REG_BASE      + 0x0000101c)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_RMSK                                                            0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_BMSK                                    0xc0000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_SHFT                                          0x1e
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_BMSK                                0x30000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_SHFT                                      0x1c
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_GLITCH_PREVENTION_DIS_BMSK                                       0x8000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_GLITCH_PREVENTION_DIS_SHFT                                            0x1b
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_FINE_FCW_BMSK                                                    0x7e00000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_FINE_FCW_SHFT                                                         0x15
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OVERRIDE_FINE_FCW_BMSK                                            0x100000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OVERRIDE_FINE_FCW_SHFT                                                0x14
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_COARSE_FCW_BMSK                                                    0xff000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_COARSE_FCW_SHFT                                                        0xc
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OVERRIDE_COARSE_FCW_BMSK                                             0x800
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OVERRIDE_COARSE_FCW_SHFT                                               0xb
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_PROCESS_CALIB_WORD_BMSK                                              0x700
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_PROCESS_CALIB_WORD_SHFT                                                0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OVERRIDE_CALIB_WORD_BMSK                                              0x80
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OVERRIDE_CALIB_WORD_SHFT                                               0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_DISABLE_LFSR_BMSK                                                     0x40
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_DISABLE_LFSR_SHFT                                                      0x6
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_EN_VREF_TEST_BMSK                                                     0x20
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_EN_VREF_TEST_SHFT                                                      0x5
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_EN_VDCO_TEST_BMSK                                                     0x10
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_EN_VDCO_TEST_SHFT                                                      0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_BYPASS_ATEST_BUFFER_BMSK                                               0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_BYPASS_ATEST_BUFFER_SHFT                                               0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_EN_IVCO_TEST_BMSK                                                      0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_EN_IVCO_TEST_SHFT                                                      0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ATEST1_EN_BMSK                                                         0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ATEST1_EN_SHFT                                                         0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ATEST0_EN_BMSK                                                         0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ATEST0_EN_SHFT                                                         0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_ADDR                                                          (LPASS_CC_REG_BASE      + 0x00001020)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_RMSK                                                          0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_ENABLE_TRAINING_SEQ_BMSK                                      0x80000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_ENABLE_TRAINING_SEQ_SHFT                                            0x1f
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_LOGIC_DEP_BMSK                                         0x40000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_LOGIC_DEP_SHFT                                               0x1e
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_BIAS_DET_BMSK                                          0x20000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_BIAS_DET_SHFT                                                0x1d
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_MSB_CLOCK_SELECT_BMSK                                         0x10000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_MSB_CLOCK_SELECT_SHFT                                               0x1c
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_BMSK                                    0x8000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_SHFT                                         0x1b
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_BMSK                                 0x6000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_SHFT                                      0x19
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_MIRROR_DEVICE_ADJUST_BMSK                                      0x1c00000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_MIRROR_DEVICE_ADJUST_SHFT                                           0x16
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_BMSK                                       0x200000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_SHFT                                           0x15
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DISABLE_CLAMP_BMSK                                              0x100000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DISABLE_CLAMP_SHFT                                                  0x14
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_MODE_OF_BIAS_BMSK                                         0x80000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_MODE_OF_BIAS_SHFT                                            0x13
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_BMSK                                     0x40000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_SHFT                                        0x12
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BIAS_ADJUST_BMSK                                                 0x30000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BIAS_ADJUST_SHFT                                                    0x10
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DIV2_NMO_EN_BMSK                                                  0x8000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DIV2_NMO_EN_SHFT                                                     0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DIS_AMP_STARTUP_BMSK                                              0x4000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DIS_AMP_STARTUP_SHFT                                                 0xe
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_SINGLE_DMET_EN_BMSK                                               0x2000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_SINGLE_DMET_EN_SHFT                                                  0xd
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DEMET_WINDOW_DIS_BMSK                                             0x1000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DEMET_WINDOW_DIS_SHFT                                                0xc
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_NMO_OSC_SEL_BMSK                                                   0xc00
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_NMO_OSC_SEL_SHFT                                                     0xa
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_NOISE_MAG_BMSK                                                     0x380
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_NOISE_MAG_SHFT                                                       0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_NOISE_GEN_EN_BMSK                                                   0x40
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_NOISE_GEN_EN_SHFT                                                    0x6
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_OSC_BIAS_GND_BMSK                                                   0x20
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_OSC_BIAS_GND_SHFT                                                    0x5
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_BMSK                                               0x18
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_SHFT                                                0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_STATUS_REG_EN_BMSK                                                   0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_STATUS_REG_EN_SHFT                                                   0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_SKIP_FINE_TUNE_BMSK                                                  0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_SKIP_FINE_TUNE_SHFT                                                  0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_SKIP_FAST_LOCK_BMSK                                                  0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_SKIP_FAST_LOCK_SHFT                                                  0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_ADDR                                                                (LPASS_CC_REG_BASE      + 0x00001000)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RMSK                                                                0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_LOCK_DET_BMSK                                                   0x80000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_LOCK_DET_SHFT                                                         0x1f
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                                                0x40000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                                                      0x1e
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_ACK_LATCH_BMSK                                                  0x20000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_ACK_LATCH_SHFT                                                        0x1d
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_LOCK_DET_FINE_BMSK                                              0x10000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_LOCK_DET_FINE_SHFT                                                    0x1c
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RESERVE_BITS27_25_BMSK                                               0xe000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RESERVE_BITS27_25_SHFT                                                    0x19
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_FSM_LEGACY_MODE_BMSK                                                 0x1000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_FSM_LEGACY_MODE_SHFT                                                      0x18
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK                                       0x800000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT                                           0x17
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_UPDATE_BMSK                                                       0x400000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_UPDATE_SHFT                                                           0x16
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                                               0x200000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                                                   0x15
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                                                 0x100000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                                                     0x14
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_BIAS_COUNT_BMSK                                                    0xfc000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_BIAS_COUNT_SHFT                                                        0xe
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_LOCK_COUNT_BMSK                                                     0x3f00
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_LOCK_COUNT_SHFT                                                        0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RESERVE_BITS7_3_BMSK                                                      0xf8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RESERVE_BITS7_3_SHFT                                                       0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_RESET_N_BMSK                                                           0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_RESET_N_SHFT                                                           0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RESERVE_BIT1_BMSK                                                          0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RESERVE_BIT1_SHFT                                                          0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_OUTCTRL_BMSK                                                           0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_OUTCTRL_SHFT                                                           0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_ADDR                                                              (LPASS_CC_REG_BASE      + 0x00001024)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_RMSK                                                              0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_STATUS_31_0_BMSK                                                  0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_STATUS_31_0_SHFT                                                         0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_ADDR                                                            (LPASS_CC_REG_BASE      + 0x00001028)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_RMSK                                                            0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_RESERVE_BITS31_27_BMSK                                          0xf8000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_RESERVE_BITS31_27_SHFT                                                0x1b
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_FCW_PCAL_BMSK                                                    0x7000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_FCW_PCAL_SHFT                                                         0x18
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_FCW_ACC_BMSK                                                      0xffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_FCW_ACC_SHFT                                                           0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_ADDR                                                              (LPASS_CC_REG_BASE      + 0x0000102c)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_RMSK                                                                     0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_PLL_OPMODE_BMSK                                                          0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_PLL_OPMODE_SHFT                                                          0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00001030)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_RMSK                                                                      0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_PLL_STATE_BMSK                                                            0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_PLL_STATE_SHFT                                                            0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00001034)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_RMSK                                                                     0xff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_DROOP_CODE_BMSK                                                          0xfe
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_DROOP_CODE_SHFT                                                           0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_DROOP_EN_BMSK                                                             0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_DROOP_EN_SHFT                                                             0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_ADDR                                                            (LPASS_CC_REG_BASE      + 0x00001038)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_RMSK                                                                0xffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_PLL_FRAC_VAL_BMSK                                                   0xffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_PLL_FRAC_VAL_SHFT                                                      0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0000103c)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_RMSK                                                                     0xff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_SPARE_OUTPUTS_BMSK                                                       0xf0
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_SPARE_OUTPUTS_SHFT                                                        0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_SPARE_INPUTS_BMSK                                                         0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_SPARE_INPUTS_SHFT                                                         0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_ADDR                                                     (LPASS_CC_REG_BASE      + 0x00001040)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_RMSK                                                           0xff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_SSC_DELTA_ALPHA_BMSK                                           0xff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_SSC_DELTA_ALPHA_SHFT                                            0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_ADDR                                                       (LPASS_CC_REG_BASE      + 0x00001048)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_RMSK                                                              0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_SSC_NUM_STEPS_BMSK                                                0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_SSC_NUM_STEPS_SHFT                                                0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_ADDR                                                     (LPASS_CC_REG_BASE      + 0x00001044)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_RMSK                                                            0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_SSC_UPDATE_RATE_BMSK                                            0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_SSC_UPDATE_RATE_SHFT                                            0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_ADDR                                                          (LPASS_CC_REG_BASE      + 0x0000104c)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_RMSK                                                                 0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_ODD_CGC_EN_BMSK                                                  0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_ODD_CGC_EN_SHFT                                                  0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_EVEN_CGC_EN_BMSK                                                 0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_EVEN_CGC_EN_SHFT                                                 0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_MAIN_SVS_CGC_EN_BMSK                                             0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_MAIN_SVS_CGC_EN_SHFT                                             0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_MAIN_CGC_EN_BMSK                                                 0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_MAIN_CGC_EN_SHFT                                                 0x0

#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_ADDR                                                  (LPASS_CC_REG_BASE      + 0x00001050)
#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_RMSK                                                         0x3
#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_IN          \
        in_dword_masked(HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_ADDR, HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_RMSK)
#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_ADDR, m)
#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_OUT(v)      \
        out_dword(HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_ADDR,v)
#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_ADDR,m,v,HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_IN)
#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_CLK_DIV_BMSK                                                 0x3
#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_CLK_DIV_SHFT                                                 0x0

#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_ADDR                                                         (LPASS_CC_REG_BASE      + 0x00002000)
#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_RMSK                                                                0x3
#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_PLL_CLK_SRC_SEL_BMSK                                                0x3
#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_PLL_CLK_SRC_SEL_SHFT                                                0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_ADDR                                                     (LPASS_CC_REG_BASE      + 0x00003000)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_RMSK                                                            0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_PLL_CLK_SRC_SEL_BMSK                                            0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_PLL_CLK_SRC_SEL_SHFT                                            0x0

#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_ADDR                                                          (LPASS_CC_REG_BASE      + 0x00004000)
#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_RMSK                                                                 0x1
#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_ADDR, HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_RMSK)
#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_ADDR, m)
#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_ADDR,v)
#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_ADDR,m,v,HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_IN)
#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_PLL_CLK_SRC_SEL_BMSK                                                 0x1
#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_PLL_CLK_SRC_SEL_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_ADDR                                                             (LPASS_CC_REG_BASE      + 0x0000b008)
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_RMSK                                                             0x80000003
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_HW_CTL_BMSK                                                             0x2
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_HW_CTL_SHFT                                                             0x1
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x0000f000)
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_RMSK                                                                  0x800000f3
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ADDR, HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_IN)
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ROOT_OFF_BMSK                                                         0x80000000
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ROOT_OFF_SHFT                                                               0x1f
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_D_BMSK                                                                0x80
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_D_SHFT                                                                 0x7
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_N_BMSK                                                                0x40
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_N_SHFT                                                                 0x6
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_M_BMSK                                                                0x20
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_M_SHFT                                                                 0x5
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                         0x10
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                          0x4
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ROOT_EN_BMSK                                                                 0x2
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ROOT_EN_SHFT                                                                 0x1
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_UPDATE_BMSK                                                                  0x1
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_UPDATE_SHFT                                                                  0x0

#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x0000f004)
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_RMSK                                                                      0x771f
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_ADDR, HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_IN)
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_ALT_SRC_SEL_BMSK                                                          0x4000
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_ALT_SRC_SEL_SHFT                                                             0xe
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_MODE_BMSK                                                                 0x3000
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_MODE_SHFT                                                                    0xc
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_SRC_SEL_BMSK                                                               0x700
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_SRC_SEL_SHFT                                                                 0x8
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_SRC_DIV_BMSK                                                                0x1f
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_SRC_DIV_SHFT                                                                 0x0

#define HWIO_LPASS_LPAIF_SPKR_M_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0000f008)
#define HWIO_LPASS_LPAIF_SPKR_M_RMSK                                                                               0xff
#define HWIO_LPASS_LPAIF_SPKR_M_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_M_ADDR, HWIO_LPASS_LPAIF_SPKR_M_RMSK)
#define HWIO_LPASS_LPAIF_SPKR_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_M_ADDR, m)
#define HWIO_LPASS_LPAIF_SPKR_M_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SPKR_M_ADDR,v)
#define HWIO_LPASS_LPAIF_SPKR_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SPKR_M_ADDR,m,v,HWIO_LPASS_LPAIF_SPKR_M_IN)
#define HWIO_LPASS_LPAIF_SPKR_M_M_BMSK                                                                             0xff
#define HWIO_LPASS_LPAIF_SPKR_M_M_SHFT                                                                              0x0

#define HWIO_LPASS_LPAIF_SPKR_N_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0000f00c)
#define HWIO_LPASS_LPAIF_SPKR_N_RMSK                                                                               0xff
#define HWIO_LPASS_LPAIF_SPKR_N_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_N_ADDR, HWIO_LPASS_LPAIF_SPKR_N_RMSK)
#define HWIO_LPASS_LPAIF_SPKR_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_N_ADDR, m)
#define HWIO_LPASS_LPAIF_SPKR_N_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SPKR_N_ADDR,v)
#define HWIO_LPASS_LPAIF_SPKR_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SPKR_N_ADDR,m,v,HWIO_LPASS_LPAIF_SPKR_N_IN)
#define HWIO_LPASS_LPAIF_SPKR_N_NOT_N_MINUS_M_BMSK                                                                 0xff
#define HWIO_LPASS_LPAIF_SPKR_N_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_LPASS_LPAIF_SPKR_D_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0000f010)
#define HWIO_LPASS_LPAIF_SPKR_D_RMSK                                                                               0xff
#define HWIO_LPASS_LPAIF_SPKR_D_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_D_ADDR, HWIO_LPASS_LPAIF_SPKR_D_RMSK)
#define HWIO_LPASS_LPAIF_SPKR_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_D_ADDR, m)
#define HWIO_LPASS_LPAIF_SPKR_D_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SPKR_D_ADDR,v)
#define HWIO_LPASS_LPAIF_SPKR_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SPKR_D_ADDR,m,v,HWIO_LPASS_LPAIF_SPKR_D_IN)
#define HWIO_LPASS_LPAIF_SPKR_D_NOT_2D_BMSK                                                                        0xff
#define HWIO_LPASS_LPAIF_SPKR_D_NOT_2D_SHFT                                                                         0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_ADDR                                                 (LPASS_CC_REG_BASE      + 0x0000f014)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_RMSK                                                 0x80000003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_CLK_OFF_BMSK                                         0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_CLK_OFF_SHFT                                               0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_HW_CTL_BMSK                                                 0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_HW_CTL_SHFT                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_CLK_ENABLE_BMSK                                             0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_CLK_ENABLE_SHFT                                             0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_ADDR                                                (LPASS_CC_REG_BASE      + 0x0000f018)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_RMSK                                                0x81ff0003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_CLK_OFF_BMSK                                        0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_CLK_OFF_SHFT                                              0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_CLK_DIV_BMSK                                         0x1ff0000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_CLK_DIV_SHFT                                              0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_HW_CTL_BMSK                                                0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_HW_CTL_SHFT                                                0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_CLK_ENABLE_BMSK                                            0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_CLK_ENABLE_SHFT                                            0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_ADDR                                                (LPASS_CC_REG_BASE      + 0x0000f01c)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_RMSK                                                0x80000003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_CLK_OFF_BMSK                                        0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_CLK_OFF_SHFT                                              0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_HW_CTL_BMSK                                                0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_HW_CTL_SHFT                                                0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_CLK_ENABLE_BMSK                                            0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_CLK_ENABLE_SHFT                                            0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_ADDR                                           (LPASS_CC_REG_BASE      + 0x0000f020)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_RMSK                                           0x80007f13
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                          0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                             0x7f00
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                0x8
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                       0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                        0x4
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                   0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_IRQ_EN_BMSK                                           0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_IRQ_EN_SHFT                                           0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_ADDR                                                  (LPASS_CC_REG_BASE      + 0x0000f024)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_RMSK                                                         0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_INV_EXT_CLK_BMSK                                             0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_INV_EXT_CLK_SHFT                                             0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_INV_INT_CLK_BMSK                                             0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_INV_INT_CLK_SHFT                                             0x0

#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00010000)
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ADDR, HWIO_LPASS_LPAIF_PRI_CMD_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_PRI_CMD_RCGR_IN)
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00010004)
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_CFG_RCGR_ADDR, HWIO_LPASS_LPAIF_PRI_CFG_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PRI_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PRI_CFG_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_PRI_CFG_RCGR_IN)
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_LPAIF_PRI_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00010008)
#define HWIO_LPASS_LPAIF_PRI_M_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_PRI_M_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_M_ADDR, HWIO_LPASS_LPAIF_PRI_M_RMSK)
#define HWIO_LPASS_LPAIF_PRI_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_M_ADDR, m)
#define HWIO_LPASS_LPAIF_PRI_M_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PRI_M_ADDR,v)
#define HWIO_LPASS_LPAIF_PRI_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PRI_M_ADDR,m,v,HWIO_LPASS_LPAIF_PRI_M_IN)
#define HWIO_LPASS_LPAIF_PRI_M_M_BMSK                                                                            0xffff
#define HWIO_LPASS_LPAIF_PRI_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_LPAIF_PRI_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0001000c)
#define HWIO_LPASS_LPAIF_PRI_N_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_PRI_N_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_N_ADDR, HWIO_LPASS_LPAIF_PRI_N_RMSK)
#define HWIO_LPASS_LPAIF_PRI_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_N_ADDR, m)
#define HWIO_LPASS_LPAIF_PRI_N_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PRI_N_ADDR,v)
#define HWIO_LPASS_LPAIF_PRI_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PRI_N_ADDR,m,v,HWIO_LPASS_LPAIF_PRI_N_IN)
#define HWIO_LPASS_LPAIF_PRI_N_NOT_N_MINUS_M_BMSK                                                                0xffff
#define HWIO_LPASS_LPAIF_PRI_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_LPAIF_PRI_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00010010)
#define HWIO_LPASS_LPAIF_PRI_D_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_PRI_D_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_D_ADDR, HWIO_LPASS_LPAIF_PRI_D_RMSK)
#define HWIO_LPASS_LPAIF_PRI_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_D_ADDR, m)
#define HWIO_LPASS_LPAIF_PRI_D_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PRI_D_ADDR,v)
#define HWIO_LPASS_LPAIF_PRI_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PRI_D_ADDR,m,v,HWIO_LPASS_LPAIF_PRI_D_IN)
#define HWIO_LPASS_LPAIF_PRI_D_NOT_2D_BMSK                                                                       0xffff
#define HWIO_LPASS_LPAIF_PRI_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_ADDR                                                       (LPASS_CC_REG_BASE      + 0x00010018)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_RMSK                                                       0x81ff0003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_CLK_DIV_BMSK                                                0x1ff0000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_CLK_DIV_SHFT                                                     0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_HW_CTL_BMSK                                                       0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_HW_CTL_SHFT                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_ADDR                                                       (LPASS_CC_REG_BASE      + 0x0001001c)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_RMSK                                                       0x80000003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_HW_CTL_BMSK                                                       0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_HW_CTL_SHFT                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_ADDR                                                         (LPASS_CC_REG_BASE      + 0x00010020)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_RMSK                                                                0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_INV_EXT_CLK_BMSK                                                    0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_INV_EXT_CLK_SHFT                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_INV_INT_CLK_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_INV_INT_CLK_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_ADDR                                                  (LPASS_CC_REG_BASE      + 0x00010024)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_RMSK                                                  0x80007f13
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                 0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                       0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                    0x7f00
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                       0x8
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                              0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                               0x4
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                          0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                          0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_IRQ_EN_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_IRQ_EN_SHFT                                                  0x0

#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00011000)
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ADDR, HWIO_LPASS_LPAIF_SEC_CMD_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_SEC_CMD_RCGR_IN)
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00011004)
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_CFG_RCGR_ADDR, HWIO_LPASS_LPAIF_SEC_CFG_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SEC_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SEC_CFG_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_SEC_CFG_RCGR_IN)
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_LPAIF_SEC_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00011008)
#define HWIO_LPASS_LPAIF_SEC_M_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_SEC_M_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_M_ADDR, HWIO_LPASS_LPAIF_SEC_M_RMSK)
#define HWIO_LPASS_LPAIF_SEC_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_M_ADDR, m)
#define HWIO_LPASS_LPAIF_SEC_M_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SEC_M_ADDR,v)
#define HWIO_LPASS_LPAIF_SEC_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SEC_M_ADDR,m,v,HWIO_LPASS_LPAIF_SEC_M_IN)
#define HWIO_LPASS_LPAIF_SEC_M_M_BMSK                                                                            0xffff
#define HWIO_LPASS_LPAIF_SEC_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_LPAIF_SEC_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0001100c)
#define HWIO_LPASS_LPAIF_SEC_N_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_SEC_N_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_N_ADDR, HWIO_LPASS_LPAIF_SEC_N_RMSK)
#define HWIO_LPASS_LPAIF_SEC_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_N_ADDR, m)
#define HWIO_LPASS_LPAIF_SEC_N_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SEC_N_ADDR,v)
#define HWIO_LPASS_LPAIF_SEC_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SEC_N_ADDR,m,v,HWIO_LPASS_LPAIF_SEC_N_IN)
#define HWIO_LPASS_LPAIF_SEC_N_NOT_N_MINUS_M_BMSK                                                                0xffff
#define HWIO_LPASS_LPAIF_SEC_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_LPAIF_SEC_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00011010)
#define HWIO_LPASS_LPAIF_SEC_D_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_SEC_D_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_D_ADDR, HWIO_LPASS_LPAIF_SEC_D_RMSK)
#define HWIO_LPASS_LPAIF_SEC_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_D_ADDR, m)
#define HWIO_LPASS_LPAIF_SEC_D_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SEC_D_ADDR,v)
#define HWIO_LPASS_LPAIF_SEC_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SEC_D_ADDR,m,v,HWIO_LPASS_LPAIF_SEC_D_IN)
#define HWIO_LPASS_LPAIF_SEC_D_NOT_2D_BMSK                                                                       0xffff
#define HWIO_LPASS_LPAIF_SEC_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_ADDR                                                       (LPASS_CC_REG_BASE      + 0x00011018)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_RMSK                                                       0x81ff0003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_CLK_DIV_BMSK                                                0x1ff0000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_CLK_DIV_SHFT                                                     0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_HW_CTL_BMSK                                                       0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_HW_CTL_SHFT                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_ADDR                                                       (LPASS_CC_REG_BASE      + 0x0001101c)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_RMSK                                                       0x80000003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_HW_CTL_BMSK                                                       0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_HW_CTL_SHFT                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_ADDR                                                         (LPASS_CC_REG_BASE      + 0x00011020)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_RMSK                                                                0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_INV_EXT_CLK_BMSK                                                    0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_INV_EXT_CLK_SHFT                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_INV_INT_CLK_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_INV_INT_CLK_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_ADDR                                                  (LPASS_CC_REG_BASE      + 0x00011024)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_RMSK                                                  0x80007f13
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                 0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                       0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                    0x7f00
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                       0x8
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                              0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                               0x4
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                          0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                          0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_IRQ_EN_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_IRQ_EN_SHFT                                                  0x0

#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00012000)
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_CMD_RCGR_ADDR, HWIO_LPASS_LPAIF_TER_CMD_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_TER_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_TER_CMD_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_TER_CMD_RCGR_IN)
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00012004)
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_CFG_RCGR_ADDR, HWIO_LPASS_LPAIF_TER_CFG_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_TER_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_TER_CFG_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_TER_CFG_RCGR_IN)
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_LPAIF_TER_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00012008)
#define HWIO_LPASS_LPAIF_TER_M_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_TER_M_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_M_ADDR, HWIO_LPASS_LPAIF_TER_M_RMSK)
#define HWIO_LPASS_LPAIF_TER_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_M_ADDR, m)
#define HWIO_LPASS_LPAIF_TER_M_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_TER_M_ADDR,v)
#define HWIO_LPASS_LPAIF_TER_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_TER_M_ADDR,m,v,HWIO_LPASS_LPAIF_TER_M_IN)
#define HWIO_LPASS_LPAIF_TER_M_M_BMSK                                                                            0xffff
#define HWIO_LPASS_LPAIF_TER_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_LPAIF_TER_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0001200c)
#define HWIO_LPASS_LPAIF_TER_N_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_TER_N_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_N_ADDR, HWIO_LPASS_LPAIF_TER_N_RMSK)
#define HWIO_LPASS_LPAIF_TER_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_N_ADDR, m)
#define HWIO_LPASS_LPAIF_TER_N_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_TER_N_ADDR,v)
#define HWIO_LPASS_LPAIF_TER_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_TER_N_ADDR,m,v,HWIO_LPASS_LPAIF_TER_N_IN)
#define HWIO_LPASS_LPAIF_TER_N_NOT_N_MINUS_M_BMSK                                                                0xffff
#define HWIO_LPASS_LPAIF_TER_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_LPAIF_TER_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00012010)
#define HWIO_LPASS_LPAIF_TER_D_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_TER_D_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_D_ADDR, HWIO_LPASS_LPAIF_TER_D_RMSK)
#define HWIO_LPASS_LPAIF_TER_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_D_ADDR, m)
#define HWIO_LPASS_LPAIF_TER_D_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_TER_D_ADDR,v)
#define HWIO_LPASS_LPAIF_TER_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_TER_D_ADDR,m,v,HWIO_LPASS_LPAIF_TER_D_IN)
#define HWIO_LPASS_LPAIF_TER_D_NOT_2D_BMSK                                                                       0xffff
#define HWIO_LPASS_LPAIF_TER_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_ADDR                                                       (LPASS_CC_REG_BASE      + 0x00012018)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_RMSK                                                       0x81ff0003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_CLK_DIV_BMSK                                                0x1ff0000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_CLK_DIV_SHFT                                                     0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_HW_CTL_BMSK                                                       0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_HW_CTL_SHFT                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_ADDR                                                       (LPASS_CC_REG_BASE      + 0x0001201c)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_RMSK                                                       0x80000003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_HW_CTL_BMSK                                                       0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_HW_CTL_SHFT                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_ADDR                                                         (LPASS_CC_REG_BASE      + 0x00012020)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_RMSK                                                                0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_INV_EXT_CLK_BMSK                                                    0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_INV_EXT_CLK_SHFT                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_INV_INT_CLK_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_INV_INT_CLK_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_ADDR                                                  (LPASS_CC_REG_BASE      + 0x00012024)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_RMSK                                                  0x80007f13
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                 0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                       0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                    0x7f00
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                       0x8
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                              0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                               0x4
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                          0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                          0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_IRQ_EN_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_IRQ_EN_SHFT                                                  0x0

#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x00013000)
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_RMSK                                                                  0x800000f3
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ADDR, HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_IN)
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ROOT_OFF_BMSK                                                         0x80000000
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ROOT_OFF_SHFT                                                               0x1f
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_D_BMSK                                                                0x80
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_D_SHFT                                                                 0x7
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_N_BMSK                                                                0x40
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_N_SHFT                                                                 0x6
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_M_BMSK                                                                0x20
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_M_SHFT                                                                 0x5
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                         0x10
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                          0x4
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ROOT_EN_BMSK                                                                 0x2
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ROOT_EN_SHFT                                                                 0x1
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_UPDATE_BMSK                                                                  0x1
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_UPDATE_SHFT                                                                  0x0

#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x00013004)
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_RMSK                                                                      0x771f
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_ADDR, HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_IN)
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_ALT_SRC_SEL_BMSK                                                          0x4000
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_ALT_SRC_SEL_SHFT                                                             0xe
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_MODE_BMSK                                                                 0x3000
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_MODE_SHFT                                                                    0xc
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_SRC_SEL_BMSK                                                               0x700
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_SRC_SEL_SHFT                                                                 0x8
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_SRC_DIV_BMSK                                                                0x1f
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_SRC_DIV_SHFT                                                                 0x0

#define HWIO_LPASS_LPAIF_QUAD_M_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x00013008)
#define HWIO_LPASS_LPAIF_QUAD_M_RMSK                                                                             0xffff
#define HWIO_LPASS_LPAIF_QUAD_M_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_M_ADDR, HWIO_LPASS_LPAIF_QUAD_M_RMSK)
#define HWIO_LPASS_LPAIF_QUAD_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_M_ADDR, m)
#define HWIO_LPASS_LPAIF_QUAD_M_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_QUAD_M_ADDR,v)
#define HWIO_LPASS_LPAIF_QUAD_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_QUAD_M_ADDR,m,v,HWIO_LPASS_LPAIF_QUAD_M_IN)
#define HWIO_LPASS_LPAIF_QUAD_M_M_BMSK                                                                           0xffff
#define HWIO_LPASS_LPAIF_QUAD_M_M_SHFT                                                                              0x0

#define HWIO_LPASS_LPAIF_QUAD_N_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0001300c)
#define HWIO_LPASS_LPAIF_QUAD_N_RMSK                                                                             0xffff
#define HWIO_LPASS_LPAIF_QUAD_N_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_N_ADDR, HWIO_LPASS_LPAIF_QUAD_N_RMSK)
#define HWIO_LPASS_LPAIF_QUAD_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_N_ADDR, m)
#define HWIO_LPASS_LPAIF_QUAD_N_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_QUAD_N_ADDR,v)
#define HWIO_LPASS_LPAIF_QUAD_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_QUAD_N_ADDR,m,v,HWIO_LPASS_LPAIF_QUAD_N_IN)
#define HWIO_LPASS_LPAIF_QUAD_N_NOT_N_MINUS_M_BMSK                                                               0xffff
#define HWIO_LPASS_LPAIF_QUAD_N_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_LPASS_LPAIF_QUAD_D_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x00013010)
#define HWIO_LPASS_LPAIF_QUAD_D_RMSK                                                                             0xffff
#define HWIO_LPASS_LPAIF_QUAD_D_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_D_ADDR, HWIO_LPASS_LPAIF_QUAD_D_RMSK)
#define HWIO_LPASS_LPAIF_QUAD_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_D_ADDR, m)
#define HWIO_LPASS_LPAIF_QUAD_D_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_QUAD_D_ADDR,v)
#define HWIO_LPASS_LPAIF_QUAD_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_QUAD_D_ADDR,m,v,HWIO_LPASS_LPAIF_QUAD_D_IN)
#define HWIO_LPASS_LPAIF_QUAD_D_NOT_2D_BMSK                                                                      0xffff
#define HWIO_LPASS_LPAIF_QUAD_D_NOT_2D_SHFT                                                                         0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x00013018)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_RMSK                                                      0x81ff0003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_CLK_DIV_BMSK                                               0x1ff0000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_CLK_DIV_SHFT                                                    0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x0001301c)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_RMSK                                                      0x80000003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00013020)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_RMSK                                                               0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_INV_EXT_CLK_BMSK                                                   0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_INV_EXT_CLK_SHFT                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_INV_INT_CLK_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_INV_INT_CLK_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_ADDR                                                 (LPASS_CC_REG_BASE      + 0x00013024)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_RMSK                                                 0x80007f13
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                   0x7f00
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                      0x8
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                             0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                              0x4
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                         0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                         0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_IRQ_EN_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_IRQ_EN_SHFT                                                 0x0

#define HWIO_LPASS_ATIME_CMD_RCGR_ADDR                                                                       (LPASS_CC_REG_BASE      + 0x00015000)
#define HWIO_LPASS_ATIME_CMD_RCGR_RMSK                                                                       0x800000f3
#define HWIO_LPASS_ATIME_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_ATIME_CMD_RCGR_ADDR, HWIO_LPASS_ATIME_CMD_RCGR_RMSK)
#define HWIO_LPASS_ATIME_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_ATIME_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_ATIME_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_ATIME_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_ATIME_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_ATIME_CMD_RCGR_ADDR,m,v,HWIO_LPASS_ATIME_CMD_RCGR_IN)
#define HWIO_LPASS_ATIME_CMD_RCGR_ROOT_OFF_BMSK                                                              0x80000000
#define HWIO_LPASS_ATIME_CMD_RCGR_ROOT_OFF_SHFT                                                                    0x1f
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_D_BMSK                                                                     0x80
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_D_SHFT                                                                      0x7
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_N_BMSK                                                                     0x40
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_N_SHFT                                                                      0x6
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_M_BMSK                                                                     0x20
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_M_SHFT                                                                      0x5
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                              0x10
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                               0x4
#define HWIO_LPASS_ATIME_CMD_RCGR_ROOT_EN_BMSK                                                                      0x2
#define HWIO_LPASS_ATIME_CMD_RCGR_ROOT_EN_SHFT                                                                      0x1
#define HWIO_LPASS_ATIME_CMD_RCGR_UPDATE_BMSK                                                                       0x1
#define HWIO_LPASS_ATIME_CMD_RCGR_UPDATE_SHFT                                                                       0x0

#define HWIO_LPASS_ATIME_CFG_RCGR_ADDR                                                                       (LPASS_CC_REG_BASE      + 0x00015004)
#define HWIO_LPASS_ATIME_CFG_RCGR_RMSK                                                                           0x771f
#define HWIO_LPASS_ATIME_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_ATIME_CFG_RCGR_ADDR, HWIO_LPASS_ATIME_CFG_RCGR_RMSK)
#define HWIO_LPASS_ATIME_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_ATIME_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_ATIME_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_ATIME_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_ATIME_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_ATIME_CFG_RCGR_ADDR,m,v,HWIO_LPASS_ATIME_CFG_RCGR_IN)
#define HWIO_LPASS_ATIME_CFG_RCGR_ALT_SRC_SEL_BMSK                                                               0x4000
#define HWIO_LPASS_ATIME_CFG_RCGR_ALT_SRC_SEL_SHFT                                                                  0xe
#define HWIO_LPASS_ATIME_CFG_RCGR_MODE_BMSK                                                                      0x3000
#define HWIO_LPASS_ATIME_CFG_RCGR_MODE_SHFT                                                                         0xc
#define HWIO_LPASS_ATIME_CFG_RCGR_SRC_SEL_BMSK                                                                    0x700
#define HWIO_LPASS_ATIME_CFG_RCGR_SRC_SEL_SHFT                                                                      0x8
#define HWIO_LPASS_ATIME_CFG_RCGR_SRC_DIV_BMSK                                                                     0x1f
#define HWIO_LPASS_ATIME_CFG_RCGR_SRC_DIV_SHFT                                                                      0x0

#define HWIO_LPASS_ATIME_M_ADDR                                                                              (LPASS_CC_REG_BASE      + 0x00015008)
#define HWIO_LPASS_ATIME_M_RMSK                                                                                    0xff
#define HWIO_LPASS_ATIME_M_IN          \
        in_dword_masked(HWIO_LPASS_ATIME_M_ADDR, HWIO_LPASS_ATIME_M_RMSK)
#define HWIO_LPASS_ATIME_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_ATIME_M_ADDR, m)
#define HWIO_LPASS_ATIME_M_OUT(v)      \
        out_dword(HWIO_LPASS_ATIME_M_ADDR,v)
#define HWIO_LPASS_ATIME_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_ATIME_M_ADDR,m,v,HWIO_LPASS_ATIME_M_IN)
#define HWIO_LPASS_ATIME_M_M_BMSK                                                                                  0xff
#define HWIO_LPASS_ATIME_M_M_SHFT                                                                                   0x0

#define HWIO_LPASS_ATIME_N_ADDR                                                                              (LPASS_CC_REG_BASE      + 0x0001500c)
#define HWIO_LPASS_ATIME_N_RMSK                                                                                    0xff
#define HWIO_LPASS_ATIME_N_IN          \
        in_dword_masked(HWIO_LPASS_ATIME_N_ADDR, HWIO_LPASS_ATIME_N_RMSK)
#define HWIO_LPASS_ATIME_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_ATIME_N_ADDR, m)
#define HWIO_LPASS_ATIME_N_OUT(v)      \
        out_dword(HWIO_LPASS_ATIME_N_ADDR,v)
#define HWIO_LPASS_ATIME_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_ATIME_N_ADDR,m,v,HWIO_LPASS_ATIME_N_IN)
#define HWIO_LPASS_ATIME_N_NOT_N_MINUS_M_BMSK                                                                      0xff
#define HWIO_LPASS_ATIME_N_NOT_N_MINUS_M_SHFT                                                                       0x0

#define HWIO_LPASS_ATIME_D_ADDR                                                                              (LPASS_CC_REG_BASE      + 0x00015010)
#define HWIO_LPASS_ATIME_D_RMSK                                                                                    0xff
#define HWIO_LPASS_ATIME_D_IN          \
        in_dword_masked(HWIO_LPASS_ATIME_D_ADDR, HWIO_LPASS_ATIME_D_RMSK)
#define HWIO_LPASS_ATIME_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_ATIME_D_ADDR, m)
#define HWIO_LPASS_ATIME_D_OUT(v)      \
        out_dword(HWIO_LPASS_ATIME_D_ADDR,v)
#define HWIO_LPASS_ATIME_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_ATIME_D_ADDR,m,v,HWIO_LPASS_ATIME_D_IN)
#define HWIO_LPASS_ATIME_D_NOT_2D_BMSK                                                                             0xff
#define HWIO_LPASS_ATIME_D_NOT_2D_SHFT                                                                              0x0

#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_ADDR                                                         (LPASS_CC_REG_BASE      + 0x00015014)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_RMSK                                                         0x80000003
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_HW_CTL_BMSK                                                         0x2
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_HW_CTL_SHFT                                                         0x1
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_LPASS_RESAMPLER_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00016000)
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CMD_RCGR_ADDR, HWIO_LPASS_RESAMPLER_CMD_RCGR_RMSK)
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_CMD_RCGR_ADDR,m,v,HWIO_LPASS_RESAMPLER_CMD_RCGR_IN)
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_RESAMPLER_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00016004)
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CFG_RCGR_ADDR, HWIO_LPASS_RESAMPLER_CFG_RCGR_RMSK)
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_CFG_RCGR_ADDR,m,v,HWIO_LPASS_RESAMPLER_CFG_RCGR_IN)
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_RESAMPLER_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00016008)
#define HWIO_LPASS_RESAMPLER_M_RMSK                                                                                0xff
#define HWIO_LPASS_RESAMPLER_M_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_M_ADDR, HWIO_LPASS_RESAMPLER_M_RMSK)
#define HWIO_LPASS_RESAMPLER_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_M_ADDR, m)
#define HWIO_LPASS_RESAMPLER_M_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_M_ADDR,v)
#define HWIO_LPASS_RESAMPLER_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_M_ADDR,m,v,HWIO_LPASS_RESAMPLER_M_IN)
#define HWIO_LPASS_RESAMPLER_M_M_BMSK                                                                              0xff
#define HWIO_LPASS_RESAMPLER_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_RESAMPLER_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0001600c)
#define HWIO_LPASS_RESAMPLER_N_RMSK                                                                                0xff
#define HWIO_LPASS_RESAMPLER_N_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_N_ADDR, HWIO_LPASS_RESAMPLER_N_RMSK)
#define HWIO_LPASS_RESAMPLER_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_N_ADDR, m)
#define HWIO_LPASS_RESAMPLER_N_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_N_ADDR,v)
#define HWIO_LPASS_RESAMPLER_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_N_ADDR,m,v,HWIO_LPASS_RESAMPLER_N_IN)
#define HWIO_LPASS_RESAMPLER_N_NOT_N_MINUS_M_BMSK                                                                  0xff
#define HWIO_LPASS_RESAMPLER_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_RESAMPLER_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00016010)
#define HWIO_LPASS_RESAMPLER_D_RMSK                                                                                0xff
#define HWIO_LPASS_RESAMPLER_D_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_D_ADDR, HWIO_LPASS_RESAMPLER_D_RMSK)
#define HWIO_LPASS_RESAMPLER_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_D_ADDR, m)
#define HWIO_LPASS_RESAMPLER_D_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_D_ADDR,v)
#define HWIO_LPASS_RESAMPLER_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_D_ADDR,m,v,HWIO_LPASS_RESAMPLER_D_IN)
#define HWIO_LPASS_RESAMPLER_D_NOT_2D_BMSK                                                                         0xff
#define HWIO_LPASS_RESAMPLER_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_ADDR                                                            (LPASS_CC_REG_BASE      + 0x00016098)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_RMSK                                                            0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_FORCE_MEM_CORE_ON_BMSK                                              0x4000
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_FORCE_MEM_CORE_ON_SHFT                                                 0xe
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                            0x2000
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                               0xd
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                           0x1000
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                              0xc
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_WAKEUP_BMSK                                                          0xf00
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_WAKEUP_SHFT                                                            0x8
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_SLEEP_BMSK                                                            0xf0
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_SLEEP_SHFT                                                             0x4
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_HW_CTL_BMSK                                                            0x2
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_HW_CTL_SHFT                                                            0x1
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_CMD_DFSR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00016014)
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_RMSK                                                                       0xbfef
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CMD_DFSR_ADDR, HWIO_LPASS_RESAMPLER_CMD_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CMD_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_CMD_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_CMD_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_CMD_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_RCG_SW_CTRL_BMSK                                                           0x8000
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_RCG_SW_CTRL_SHFT                                                              0xf
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_SW_PERF_STATE_BMSK                                                         0x3800
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_SW_PERF_STATE_SHFT                                                            0xb
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_SW_OVERRIDE_BMSK                                                            0x400
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_SW_OVERRIDE_SHFT                                                              0xa
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_PERF_STATE_UPDATE_STATUS_BMSK                                               0x200
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_PERF_STATE_UPDATE_STATUS_SHFT                                                 0x9
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_DFS_FSM_STATE_BMSK                                                          0x1c0
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_DFS_FSM_STATE_SHFT                                                            0x6
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_HW_CLK_CONTROL_BMSK                                                          0x20
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_HW_CLK_CONTROL_SHFT                                                           0x5
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_CURR_PERF_STATE_BMSK                                                          0xe
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_CURR_PERF_STATE_SHFT                                                          0x1
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_DFS_EN_BMSK                                                                   0x1
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_DFS_EN_SHFT                                                                   0x0

#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00016018)
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF0_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF0_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF0_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF0_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x0001601c)
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF1_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF1_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF1_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF1_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00016020)
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF2_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF2_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF2_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF2_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00016024)
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF3_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF3_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF3_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF3_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00016028)
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF4_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF4_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF4_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF4_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x0001602c)
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF5_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF5_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF5_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF5_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00016030)
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF6_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF6_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF6_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF6_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00016034)
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF7_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF7_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF7_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF7_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016038)
#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0001603c)
#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016040)
#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016044)
#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016048)
#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0001604c)
#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016050)
#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016054)
#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016058)
#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0001605c)
#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016060)
#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016064)
#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016068)
#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0001606c)
#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016070)
#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016074)
#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016078)
#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0001607c)
#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016080)
#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016084)
#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016088)
#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0001608c)
#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016090)
#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016094)
#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00017000)
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_RMSK                                                                 0x800000f3
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ADDR, HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_RMSK)
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ADDR,m,v,HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_IN)
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ROOT_OFF_BMSK                                                        0x80000000
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ROOT_OFF_SHFT                                                              0x1f
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_D_BMSK                                                               0x80
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_D_SHFT                                                                0x7
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_N_BMSK                                                               0x40
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_N_SHFT                                                                0x6
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_M_BMSK                                                               0x20
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_M_SHFT                                                                0x5
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                        0x10
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                         0x4
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ROOT_EN_BMSK                                                                0x2
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ROOT_EN_SHFT                                                                0x1
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_UPDATE_BMSK                                                                 0x1
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_UPDATE_SHFT                                                                 0x0

#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00017004)
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_RMSK                                                                     0x771f
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_ADDR, HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_RMSK)
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_ADDR,m,v,HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_IN)
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_ALT_SRC_SEL_BMSK                                                         0x4000
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_ALT_SRC_SEL_SHFT                                                            0xe
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_AUD_SLIMBUS_M_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x00017008)
#define HWIO_LPASS_AUD_SLIMBUS_M_RMSK                                                                              0xff
#define HWIO_LPASS_AUD_SLIMBUS_M_IN          \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_M_ADDR, HWIO_LPASS_AUD_SLIMBUS_M_RMSK)
#define HWIO_LPASS_AUD_SLIMBUS_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_M_ADDR, m)
#define HWIO_LPASS_AUD_SLIMBUS_M_OUT(v)      \
        out_dword(HWIO_LPASS_AUD_SLIMBUS_M_ADDR,v)
#define HWIO_LPASS_AUD_SLIMBUS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUD_SLIMBUS_M_ADDR,m,v,HWIO_LPASS_AUD_SLIMBUS_M_IN)
#define HWIO_LPASS_AUD_SLIMBUS_M_M_BMSK                                                                            0xff
#define HWIO_LPASS_AUD_SLIMBUS_M_M_SHFT                                                                             0x0

#define HWIO_LPASS_AUD_SLIMBUS_N_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x0001700c)
#define HWIO_LPASS_AUD_SLIMBUS_N_RMSK                                                                              0xff
#define HWIO_LPASS_AUD_SLIMBUS_N_IN          \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_N_ADDR, HWIO_LPASS_AUD_SLIMBUS_N_RMSK)
#define HWIO_LPASS_AUD_SLIMBUS_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_N_ADDR, m)
#define HWIO_LPASS_AUD_SLIMBUS_N_OUT(v)      \
        out_dword(HWIO_LPASS_AUD_SLIMBUS_N_ADDR,v)
#define HWIO_LPASS_AUD_SLIMBUS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUD_SLIMBUS_N_ADDR,m,v,HWIO_LPASS_AUD_SLIMBUS_N_IN)
#define HWIO_LPASS_AUD_SLIMBUS_N_NOT_N_MINUS_M_BMSK                                                                0xff
#define HWIO_LPASS_AUD_SLIMBUS_N_NOT_N_MINUS_M_SHFT                                                                 0x0

#define HWIO_LPASS_AUD_SLIMBUS_D_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x00017010)
#define HWIO_LPASS_AUD_SLIMBUS_D_RMSK                                                                              0xff
#define HWIO_LPASS_AUD_SLIMBUS_D_IN          \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_D_ADDR, HWIO_LPASS_AUD_SLIMBUS_D_RMSK)
#define HWIO_LPASS_AUD_SLIMBUS_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_D_ADDR, m)
#define HWIO_LPASS_AUD_SLIMBUS_D_OUT(v)      \
        out_dword(HWIO_LPASS_AUD_SLIMBUS_D_ADDR,v)
#define HWIO_LPASS_AUD_SLIMBUS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUD_SLIMBUS_D_ADDR,m,v,HWIO_LPASS_AUD_SLIMBUS_D_IN)
#define HWIO_LPASS_AUD_SLIMBUS_D_NOT_2D_BMSK                                                                       0xff
#define HWIO_LPASS_AUD_SLIMBUS_D_NOT_2D_SHFT                                                                        0x0

#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_ADDR                                                          (LPASS_CC_REG_BASE      + 0x00017014)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_RMSK                                                          0x80000003
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_HW_CTL_BMSK                                                          0x2
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_HW_CTL_SHFT                                                          0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_ADDR                                                     (LPASS_CC_REG_BASE      + 0x00017018)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_RMSK                                                     0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                       0x4000
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                          0xe
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                     0x2000
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                        0xd
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                    0x1000
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                       0xc
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_WAKEUP_BMSK                                                   0xf00
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_WAKEUP_SHFT                                                     0x8
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_SLEEP_BMSK                                                     0xf0
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_SLEEP_SHFT                                                      0x4
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_HW_CTL_BMSK                                                     0x2
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_HW_CTL_SHFT                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x0001701c)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_RMSK                                                      0x80000003
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00018000)
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_RMSK                                                                 0x800000f3
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ADDR, HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_RMSK)
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ADDR,m,v,HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_IN)
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ROOT_OFF_BMSK                                                        0x80000000
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ROOT_OFF_SHFT                                                              0x1f
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_D_BMSK                                                               0x80
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_D_SHFT                                                                0x7
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_N_BMSK                                                               0x40
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_N_SHFT                                                                0x6
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_M_BMSK                                                               0x20
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_M_SHFT                                                                0x5
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                        0x10
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                         0x4
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ROOT_EN_BMSK                                                                0x2
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ROOT_EN_SHFT                                                                0x1
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_UPDATE_BMSK                                                                 0x1
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_UPDATE_SHFT                                                                 0x0

#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00018004)
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_RMSK                                                                     0x771f
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_ADDR, HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_RMSK)
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_ADDR,m,v,HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_IN)
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_ALT_SRC_SEL_BMSK                                                         0x4000
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_ALT_SRC_SEL_SHFT                                                            0xe
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_QCA_SLIMBUS_M_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x00018008)
#define HWIO_LPASS_QCA_SLIMBUS_M_RMSK                                                                              0xff
#define HWIO_LPASS_QCA_SLIMBUS_M_IN          \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_M_ADDR, HWIO_LPASS_QCA_SLIMBUS_M_RMSK)
#define HWIO_LPASS_QCA_SLIMBUS_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_M_ADDR, m)
#define HWIO_LPASS_QCA_SLIMBUS_M_OUT(v)      \
        out_dword(HWIO_LPASS_QCA_SLIMBUS_M_ADDR,v)
#define HWIO_LPASS_QCA_SLIMBUS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QCA_SLIMBUS_M_ADDR,m,v,HWIO_LPASS_QCA_SLIMBUS_M_IN)
#define HWIO_LPASS_QCA_SLIMBUS_M_M_BMSK                                                                            0xff
#define HWIO_LPASS_QCA_SLIMBUS_M_M_SHFT                                                                             0x0

#define HWIO_LPASS_QCA_SLIMBUS_N_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x0001800c)
#define HWIO_LPASS_QCA_SLIMBUS_N_RMSK                                                                              0xff
#define HWIO_LPASS_QCA_SLIMBUS_N_IN          \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_N_ADDR, HWIO_LPASS_QCA_SLIMBUS_N_RMSK)
#define HWIO_LPASS_QCA_SLIMBUS_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_N_ADDR, m)
#define HWIO_LPASS_QCA_SLIMBUS_N_OUT(v)      \
        out_dword(HWIO_LPASS_QCA_SLIMBUS_N_ADDR,v)
#define HWIO_LPASS_QCA_SLIMBUS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QCA_SLIMBUS_N_ADDR,m,v,HWIO_LPASS_QCA_SLIMBUS_N_IN)
#define HWIO_LPASS_QCA_SLIMBUS_N_NOT_N_MINUS_M_BMSK                                                                0xff
#define HWIO_LPASS_QCA_SLIMBUS_N_NOT_N_MINUS_M_SHFT                                                                 0x0

#define HWIO_LPASS_QCA_SLIMBUS_D_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x00018010)
#define HWIO_LPASS_QCA_SLIMBUS_D_RMSK                                                                              0xff
#define HWIO_LPASS_QCA_SLIMBUS_D_IN          \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_D_ADDR, HWIO_LPASS_QCA_SLIMBUS_D_RMSK)
#define HWIO_LPASS_QCA_SLIMBUS_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_D_ADDR, m)
#define HWIO_LPASS_QCA_SLIMBUS_D_OUT(v)      \
        out_dword(HWIO_LPASS_QCA_SLIMBUS_D_ADDR,v)
#define HWIO_LPASS_QCA_SLIMBUS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QCA_SLIMBUS_D_ADDR,m,v,HWIO_LPASS_QCA_SLIMBUS_D_IN)
#define HWIO_LPASS_QCA_SLIMBUS_D_NOT_2D_BMSK                                                                       0xff
#define HWIO_LPASS_QCA_SLIMBUS_D_NOT_2D_SHFT                                                                        0x0

#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_ADDR                                                          (LPASS_CC_REG_BASE      + 0x00018014)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_RMSK                                                          0x80000003
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_HW_CTL_BMSK                                                          0x2
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_HW_CTL_SHFT                                                          0x1
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_ADDR                                                     (LPASS_CC_REG_BASE      + 0x00018018)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_RMSK                                                     0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                       0x4000
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                          0xe
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                     0x2000
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                        0xd
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                    0x1000
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                       0xc
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_WAKEUP_BMSK                                                   0xf00
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_WAKEUP_SHFT                                                     0x8
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_SLEEP_BMSK                                                     0xf0
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_SLEEP_SHFT                                                      0x4
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_HW_CTL_BMSK                                                     0x2
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_HW_CTL_SHFT                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00019000)
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_RMSK                                                                 0x800000f3
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ADDR, HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_IN)
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ROOT_OFF_BMSK                                                        0x80000000
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ROOT_OFF_SHFT                                                              0x1f
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_D_BMSK                                                               0x80
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_D_SHFT                                                                0x7
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_N_BMSK                                                               0x40
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_N_SHFT                                                                0x6
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_M_BMSK                                                               0x20
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_M_SHFT                                                                0x5
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                        0x10
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                         0x4
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ROOT_EN_BMSK                                                                0x2
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ROOT_EN_SHFT                                                                0x1
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_UPDATE_BMSK                                                                 0x1
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_UPDATE_SHFT                                                                 0x0

#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00019004)
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_RMSK                                                                     0x771f
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_ADDR, HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_IN)
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_ALT_SRC_SEL_BMSK                                                         0x4000
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_ALT_SRC_SEL_SHFT                                                            0xe
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_LPAIF_PCMOE_M_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x00019008)
#define HWIO_LPASS_LPAIF_PCMOE_M_RMSK                                                                              0xff
#define HWIO_LPASS_LPAIF_PCMOE_M_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_M_ADDR, HWIO_LPASS_LPAIF_PCMOE_M_RMSK)
#define HWIO_LPASS_LPAIF_PCMOE_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_M_ADDR, m)
#define HWIO_LPASS_LPAIF_PCMOE_M_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PCMOE_M_ADDR,v)
#define HWIO_LPASS_LPAIF_PCMOE_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PCMOE_M_ADDR,m,v,HWIO_LPASS_LPAIF_PCMOE_M_IN)
#define HWIO_LPASS_LPAIF_PCMOE_M_M_BMSK                                                                            0xff
#define HWIO_LPASS_LPAIF_PCMOE_M_M_SHFT                                                                             0x0

#define HWIO_LPASS_LPAIF_PCMOE_N_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x0001900c)
#define HWIO_LPASS_LPAIF_PCMOE_N_RMSK                                                                              0xff
#define HWIO_LPASS_LPAIF_PCMOE_N_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_N_ADDR, HWIO_LPASS_LPAIF_PCMOE_N_RMSK)
#define HWIO_LPASS_LPAIF_PCMOE_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_N_ADDR, m)
#define HWIO_LPASS_LPAIF_PCMOE_N_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PCMOE_N_ADDR,v)
#define HWIO_LPASS_LPAIF_PCMOE_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PCMOE_N_ADDR,m,v,HWIO_LPASS_LPAIF_PCMOE_N_IN)
#define HWIO_LPASS_LPAIF_PCMOE_N_NOT_N_MINUS_M_BMSK                                                                0xff
#define HWIO_LPASS_LPAIF_PCMOE_N_NOT_N_MINUS_M_SHFT                                                                 0x0

#define HWIO_LPASS_LPAIF_PCMOE_D_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x00019010)
#define HWIO_LPASS_LPAIF_PCMOE_D_RMSK                                                                              0xff
#define HWIO_LPASS_LPAIF_PCMOE_D_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_D_ADDR, HWIO_LPASS_LPAIF_PCMOE_D_RMSK)
#define HWIO_LPASS_LPAIF_PCMOE_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_D_ADDR, m)
#define HWIO_LPASS_LPAIF_PCMOE_D_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PCMOE_D_ADDR,v)
#define HWIO_LPASS_LPAIF_PCMOE_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PCMOE_D_ADDR,m,v,HWIO_LPASS_LPAIF_PCMOE_D_IN)
#define HWIO_LPASS_LPAIF_PCMOE_D_NOT_2D_BMSK                                                                       0xff
#define HWIO_LPASS_LPAIF_PCMOE_D_NOT_2D_SHFT                                                                        0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_ADDR                                                    (LPASS_CC_REG_BASE      + 0x00019014)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_RMSK                                                    0x80000003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_CLK_OFF_BMSK                                            0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_CLK_OFF_SHFT                                                  0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_HW_CTL_BMSK                                                    0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_HW_CTL_SHFT                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_CLK_ENABLE_BMSK                                                0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_CLK_ENABLE_SHFT                                                0x0

#define HWIO_LPASS_SLEEP_CMD_RCGR_ADDR                                                                       (LPASS_CC_REG_BASE      + 0x0001a000)
#define HWIO_LPASS_SLEEP_CMD_RCGR_RMSK                                                                       0x800000f3
#define HWIO_LPASS_SLEEP_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_SLEEP_CMD_RCGR_ADDR, HWIO_LPASS_SLEEP_CMD_RCGR_RMSK)
#define HWIO_LPASS_SLEEP_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_SLEEP_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_SLEEP_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_SLEEP_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_SLEEP_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SLEEP_CMD_RCGR_ADDR,m,v,HWIO_LPASS_SLEEP_CMD_RCGR_IN)
#define HWIO_LPASS_SLEEP_CMD_RCGR_ROOT_OFF_BMSK                                                              0x80000000
#define HWIO_LPASS_SLEEP_CMD_RCGR_ROOT_OFF_SHFT                                                                    0x1f
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_D_BMSK                                                                     0x80
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_D_SHFT                                                                      0x7
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_N_BMSK                                                                     0x40
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_N_SHFT                                                                      0x6
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_M_BMSK                                                                     0x20
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_M_SHFT                                                                      0x5
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                              0x10
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                               0x4
#define HWIO_LPASS_SLEEP_CMD_RCGR_ROOT_EN_BMSK                                                                      0x2
#define HWIO_LPASS_SLEEP_CMD_RCGR_ROOT_EN_SHFT                                                                      0x1
#define HWIO_LPASS_SLEEP_CMD_RCGR_UPDATE_BMSK                                                                       0x1
#define HWIO_LPASS_SLEEP_CMD_RCGR_UPDATE_SHFT                                                                       0x0

#define HWIO_LPASS_SLEEP_CFG_RCGR_ADDR                                                                       (LPASS_CC_REG_BASE      + 0x0001a004)
#define HWIO_LPASS_SLEEP_CFG_RCGR_RMSK                                                                          0x1371f
#define HWIO_LPASS_SLEEP_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_SLEEP_CFG_RCGR_ADDR, HWIO_LPASS_SLEEP_CFG_RCGR_RMSK)
#define HWIO_LPASS_SLEEP_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_SLEEP_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_SLEEP_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_SLEEP_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_SLEEP_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SLEEP_CFG_RCGR_ADDR,m,v,HWIO_LPASS_SLEEP_CFG_RCGR_IN)
#define HWIO_LPASS_SLEEP_CFG_RCGR_RCGLITE_DISABLE_BMSK                                                          0x10000
#define HWIO_LPASS_SLEEP_CFG_RCGR_RCGLITE_DISABLE_SHFT                                                             0x10
#define HWIO_LPASS_SLEEP_CFG_RCGR_MODE_BMSK                                                                      0x3000
#define HWIO_LPASS_SLEEP_CFG_RCGR_MODE_SHFT                                                                         0xc
#define HWIO_LPASS_SLEEP_CFG_RCGR_SRC_SEL_BMSK                                                                    0x700
#define HWIO_LPASS_SLEEP_CFG_RCGR_SRC_SEL_SHFT                                                                      0x8
#define HWIO_LPASS_SLEEP_CFG_RCGR_SRC_DIV_BMSK                                                                     0x1f
#define HWIO_LPASS_SLEEP_CFG_RCGR_SRC_DIV_SHFT                                                                      0x0

#define HWIO_LPASS_XO_CMD_RCGR_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0001b000)
#define HWIO_LPASS_XO_CMD_RCGR_RMSK                                                                          0x800000f3
#define HWIO_LPASS_XO_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_XO_CMD_RCGR_ADDR, HWIO_LPASS_XO_CMD_RCGR_RMSK)
#define HWIO_LPASS_XO_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_XO_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_XO_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_XO_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_XO_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_XO_CMD_RCGR_ADDR,m,v,HWIO_LPASS_XO_CMD_RCGR_IN)
#define HWIO_LPASS_XO_CMD_RCGR_ROOT_OFF_BMSK                                                                 0x80000000
#define HWIO_LPASS_XO_CMD_RCGR_ROOT_OFF_SHFT                                                                       0x1f
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_D_BMSK                                                                        0x80
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_D_SHFT                                                                         0x7
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_N_BMSK                                                                        0x40
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_N_SHFT                                                                         0x6
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_M_BMSK                                                                        0x20
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_M_SHFT                                                                         0x5
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                                 0x10
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                                  0x4
#define HWIO_LPASS_XO_CMD_RCGR_ROOT_EN_BMSK                                                                         0x2
#define HWIO_LPASS_XO_CMD_RCGR_ROOT_EN_SHFT                                                                         0x1
#define HWIO_LPASS_XO_CMD_RCGR_UPDATE_BMSK                                                                          0x1
#define HWIO_LPASS_XO_CMD_RCGR_UPDATE_SHFT                                                                          0x0

#define HWIO_LPASS_XO_CFG_RCGR_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0001b004)
#define HWIO_LPASS_XO_CFG_RCGR_RMSK                                                                             0x1771f
#define HWIO_LPASS_XO_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_XO_CFG_RCGR_ADDR, HWIO_LPASS_XO_CFG_RCGR_RMSK)
#define HWIO_LPASS_XO_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_XO_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_XO_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_XO_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_XO_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_XO_CFG_RCGR_ADDR,m,v,HWIO_LPASS_XO_CFG_RCGR_IN)
#define HWIO_LPASS_XO_CFG_RCGR_RCGLITE_DISABLE_BMSK                                                             0x10000
#define HWIO_LPASS_XO_CFG_RCGR_RCGLITE_DISABLE_SHFT                                                                0x10
#define HWIO_LPASS_XO_CFG_RCGR_ALT_SRC_SEL_BMSK                                                                  0x4000
#define HWIO_LPASS_XO_CFG_RCGR_ALT_SRC_SEL_SHFT                                                                     0xe
#define HWIO_LPASS_XO_CFG_RCGR_MODE_BMSK                                                                         0x3000
#define HWIO_LPASS_XO_CFG_RCGR_MODE_SHFT                                                                            0xc
#define HWIO_LPASS_XO_CFG_RCGR_SRC_SEL_BMSK                                                                       0x700
#define HWIO_LPASS_XO_CFG_RCGR_SRC_SEL_SHFT                                                                         0x8
#define HWIO_LPASS_XO_CFG_RCGR_SRC_DIV_BMSK                                                                        0x1f
#define HWIO_LPASS_XO_CFG_RCGR_SRC_DIV_SHFT                                                                         0x0

#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_ADDR                                                           (LPASS_CC_REG_BASE      + 0x0001c000)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_RMSK                                                           0x80000003
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_HW_CTL_BMSK                                                           0x2
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_HW_CTL_SHFT                                                           0x1
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_LPASS_CORE_CMD_RCGR_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x0001d000)
#define HWIO_LPASS_CORE_CMD_RCGR_RMSK                                                                        0x800000f3
#define HWIO_LPASS_CORE_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_CMD_RCGR_ADDR, HWIO_LPASS_CORE_CMD_RCGR_RMSK)
#define HWIO_LPASS_CORE_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_CORE_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_CORE_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_CMD_RCGR_ADDR,m,v,HWIO_LPASS_CORE_CMD_RCGR_IN)
#define HWIO_LPASS_CORE_CMD_RCGR_ROOT_OFF_BMSK                                                               0x80000000
#define HWIO_LPASS_CORE_CMD_RCGR_ROOT_OFF_SHFT                                                                     0x1f
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_D_BMSK                                                                      0x80
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_D_SHFT                                                                       0x7
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_N_BMSK                                                                      0x40
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_N_SHFT                                                                       0x6
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_M_BMSK                                                                      0x20
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_M_SHFT                                                                       0x5
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                               0x10
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                                0x4
#define HWIO_LPASS_CORE_CMD_RCGR_ROOT_EN_BMSK                                                                       0x2
#define HWIO_LPASS_CORE_CMD_RCGR_ROOT_EN_SHFT                                                                       0x1
#define HWIO_LPASS_CORE_CMD_RCGR_UPDATE_BMSK                                                                        0x1
#define HWIO_LPASS_CORE_CMD_RCGR_UPDATE_SHFT                                                                        0x0

#define HWIO_LPASS_CORE_CFG_RCGR_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x0001d004)
#define HWIO_LPASS_CORE_CFG_RCGR_RMSK                                                                            0x771f
#define HWIO_LPASS_CORE_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_CFG_RCGR_ADDR, HWIO_LPASS_CORE_CFG_RCGR_RMSK)
#define HWIO_LPASS_CORE_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_CORE_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_CORE_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_CFG_RCGR_ADDR,m,v,HWIO_LPASS_CORE_CFG_RCGR_IN)
#define HWIO_LPASS_CORE_CFG_RCGR_ALT_SRC_SEL_BMSK                                                                0x4000
#define HWIO_LPASS_CORE_CFG_RCGR_ALT_SRC_SEL_SHFT                                                                   0xe
#define HWIO_LPASS_CORE_CFG_RCGR_MODE_BMSK                                                                       0x3000
#define HWIO_LPASS_CORE_CFG_RCGR_MODE_SHFT                                                                          0xc
#define HWIO_LPASS_CORE_CFG_RCGR_SRC_SEL_BMSK                                                                     0x700
#define HWIO_LPASS_CORE_CFG_RCGR_SRC_SEL_SHFT                                                                       0x8
#define HWIO_LPASS_CORE_CFG_RCGR_SRC_DIV_BMSK                                                                      0x1f
#define HWIO_LPASS_CORE_CFG_RCGR_SRC_DIV_SHFT                                                                       0x0

#define HWIO_LPASS_CORE_M_ADDR                                                                               (LPASS_CC_REG_BASE      + 0x0001d008)
#define HWIO_LPASS_CORE_M_RMSK                                                                                     0xff
#define HWIO_LPASS_CORE_M_IN          \
        in_dword_masked(HWIO_LPASS_CORE_M_ADDR, HWIO_LPASS_CORE_M_RMSK)
#define HWIO_LPASS_CORE_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_M_ADDR, m)
#define HWIO_LPASS_CORE_M_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_M_ADDR,v)
#define HWIO_LPASS_CORE_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_M_ADDR,m,v,HWIO_LPASS_CORE_M_IN)
#define HWIO_LPASS_CORE_M_M_BMSK                                                                                   0xff
#define HWIO_LPASS_CORE_M_M_SHFT                                                                                    0x0

#define HWIO_LPASS_CORE_N_ADDR                                                                               (LPASS_CC_REG_BASE      + 0x0001d00c)
#define HWIO_LPASS_CORE_N_RMSK                                                                                     0xff
#define HWIO_LPASS_CORE_N_IN          \
        in_dword_masked(HWIO_LPASS_CORE_N_ADDR, HWIO_LPASS_CORE_N_RMSK)
#define HWIO_LPASS_CORE_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_N_ADDR, m)
#define HWIO_LPASS_CORE_N_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_N_ADDR,v)
#define HWIO_LPASS_CORE_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_N_ADDR,m,v,HWIO_LPASS_CORE_N_IN)
#define HWIO_LPASS_CORE_N_NOT_N_MINUS_M_BMSK                                                                       0xff
#define HWIO_LPASS_CORE_N_NOT_N_MINUS_M_SHFT                                                                        0x0

#define HWIO_LPASS_CORE_D_ADDR                                                                               (LPASS_CC_REG_BASE      + 0x0001d010)
#define HWIO_LPASS_CORE_D_RMSK                                                                                     0xff
#define HWIO_LPASS_CORE_D_IN          \
        in_dword_masked(HWIO_LPASS_CORE_D_ADDR, HWIO_LPASS_CORE_D_RMSK)
#define HWIO_LPASS_CORE_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_D_ADDR, m)
#define HWIO_LPASS_CORE_D_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_D_ADDR,v)
#define HWIO_LPASS_CORE_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_D_ADDR,m,v,HWIO_LPASS_CORE_D_IN)
#define HWIO_LPASS_CORE_D_NOT_2D_BMSK                                                                              0xff
#define HWIO_LPASS_CORE_D_NOT_2D_SHFT                                                                               0x0

#define HWIO_LPASS_CORE_CMD_DFSR_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x0001d014)
#define HWIO_LPASS_CORE_CMD_DFSR_RMSK                                                                            0xbfef
#define HWIO_LPASS_CORE_CMD_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_CMD_DFSR_ADDR, HWIO_LPASS_CORE_CMD_DFSR_RMSK)
#define HWIO_LPASS_CORE_CMD_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_CMD_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_CMD_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_CMD_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_CMD_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_CMD_DFSR_ADDR,m,v,HWIO_LPASS_CORE_CMD_DFSR_IN)
#define HWIO_LPASS_CORE_CMD_DFSR_RCG_SW_CTRL_BMSK                                                                0x8000
#define HWIO_LPASS_CORE_CMD_DFSR_RCG_SW_CTRL_SHFT                                                                   0xf
#define HWIO_LPASS_CORE_CMD_DFSR_SW_PERF_STATE_BMSK                                                              0x3800
#define HWIO_LPASS_CORE_CMD_DFSR_SW_PERF_STATE_SHFT                                                                 0xb
#define HWIO_LPASS_CORE_CMD_DFSR_SW_OVERRIDE_BMSK                                                                 0x400
#define HWIO_LPASS_CORE_CMD_DFSR_SW_OVERRIDE_SHFT                                                                   0xa
#define HWIO_LPASS_CORE_CMD_DFSR_PERF_STATE_UPDATE_STATUS_BMSK                                                    0x200
#define HWIO_LPASS_CORE_CMD_DFSR_PERF_STATE_UPDATE_STATUS_SHFT                                                      0x9
#define HWIO_LPASS_CORE_CMD_DFSR_DFS_FSM_STATE_BMSK                                                               0x1c0
#define HWIO_LPASS_CORE_CMD_DFSR_DFS_FSM_STATE_SHFT                                                                 0x6
#define HWIO_LPASS_CORE_CMD_DFSR_HW_CLK_CONTROL_BMSK                                                               0x20
#define HWIO_LPASS_CORE_CMD_DFSR_HW_CLK_CONTROL_SHFT                                                                0x5
#define HWIO_LPASS_CORE_CMD_DFSR_CURR_PERF_STATE_BMSK                                                               0xe
#define HWIO_LPASS_CORE_CMD_DFSR_CURR_PERF_STATE_SHFT                                                               0x1
#define HWIO_LPASS_CORE_CMD_DFSR_DFS_EN_BMSK                                                                        0x1
#define HWIO_LPASS_CORE_CMD_DFSR_DFS_EN_SHFT                                                                        0x0

#define HWIO_LPASS_CORE_PERF0_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d018)
#define HWIO_LPASS_CORE_PERF0_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF0_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_DFSR_ADDR, HWIO_LPASS_CORE_PERF0_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF0_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF0_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF0_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF0_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF0_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF0_DFSR_IN)
#define HWIO_LPASS_CORE_PERF0_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF0_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF0_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF0_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF0_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF0_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF1_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d01c)
#define HWIO_LPASS_CORE_PERF1_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF1_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_DFSR_ADDR, HWIO_LPASS_CORE_PERF1_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF1_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF1_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF1_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF1_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF1_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF1_DFSR_IN)
#define HWIO_LPASS_CORE_PERF1_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF1_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF1_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF1_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF1_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF1_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF2_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d020)
#define HWIO_LPASS_CORE_PERF2_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF2_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_DFSR_ADDR, HWIO_LPASS_CORE_PERF2_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF2_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF2_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF2_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF2_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF2_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF2_DFSR_IN)
#define HWIO_LPASS_CORE_PERF2_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF2_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF2_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF2_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF2_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF2_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF3_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d024)
#define HWIO_LPASS_CORE_PERF3_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF3_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_DFSR_ADDR, HWIO_LPASS_CORE_PERF3_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF3_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF3_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF3_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF3_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF3_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF3_DFSR_IN)
#define HWIO_LPASS_CORE_PERF3_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF3_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF3_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF3_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF3_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF3_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF4_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d028)
#define HWIO_LPASS_CORE_PERF4_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF4_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_DFSR_ADDR, HWIO_LPASS_CORE_PERF4_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF4_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF4_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF4_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF4_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF4_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF4_DFSR_IN)
#define HWIO_LPASS_CORE_PERF4_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF4_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF4_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF4_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF4_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF4_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF5_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d02c)
#define HWIO_LPASS_CORE_PERF5_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF5_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_DFSR_ADDR, HWIO_LPASS_CORE_PERF5_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF5_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF5_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF5_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF5_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF5_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF5_DFSR_IN)
#define HWIO_LPASS_CORE_PERF5_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF5_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF5_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF5_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF5_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF5_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF6_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d030)
#define HWIO_LPASS_CORE_PERF6_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF6_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_DFSR_ADDR, HWIO_LPASS_CORE_PERF6_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF6_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF6_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF6_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF6_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF6_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF6_DFSR_IN)
#define HWIO_LPASS_CORE_PERF6_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF6_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF6_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF6_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF6_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF6_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF7_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d034)
#define HWIO_LPASS_CORE_PERF7_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF7_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_DFSR_ADDR, HWIO_LPASS_CORE_PERF7_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF7_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF7_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF7_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF7_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF7_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF7_DFSR_IN)
#define HWIO_LPASS_CORE_PERF7_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF7_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF7_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF7_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF7_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF7_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF0_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d038)
#define HWIO_LPASS_CORE_PERF0_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF0_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF0_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF0_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF0_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF0_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF0_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF0_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF0_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF0_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF0_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF1_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d03c)
#define HWIO_LPASS_CORE_PERF1_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF1_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF1_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF1_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF1_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF1_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF1_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF1_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF1_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF1_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF1_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF2_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d040)
#define HWIO_LPASS_CORE_PERF2_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF2_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF2_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF2_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF2_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF2_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF2_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF2_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF2_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF2_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF2_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF3_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d044)
#define HWIO_LPASS_CORE_PERF3_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF3_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF3_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF3_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF3_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF3_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF3_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF3_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF3_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF3_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF3_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF4_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d048)
#define HWIO_LPASS_CORE_PERF4_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF4_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF4_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF4_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF4_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF4_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF4_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF4_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF4_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF4_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF4_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF5_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d04c)
#define HWIO_LPASS_CORE_PERF5_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF5_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF5_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF5_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF5_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF5_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF5_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF5_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF5_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF5_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF5_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF6_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d050)
#define HWIO_LPASS_CORE_PERF6_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF6_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF6_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF6_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF6_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF6_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF6_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF6_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF6_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF6_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF6_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF7_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d054)
#define HWIO_LPASS_CORE_PERF7_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF7_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF7_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF7_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF7_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF7_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF7_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF7_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF7_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF7_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF7_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF0_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d058)
#define HWIO_LPASS_CORE_PERF0_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF0_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF0_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF0_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF0_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF0_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF0_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF0_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF0_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF0_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF0_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF1_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d05c)
#define HWIO_LPASS_CORE_PERF1_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF1_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF1_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF1_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF1_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF1_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF1_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF1_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF1_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF1_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF1_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF2_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d060)
#define HWIO_LPASS_CORE_PERF2_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF2_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF2_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF2_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF2_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF2_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF2_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF2_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF2_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF2_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF2_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF3_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d064)
#define HWIO_LPASS_CORE_PERF3_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF3_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF3_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF3_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF3_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF3_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF3_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF3_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF3_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF3_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF3_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF4_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d068)
#define HWIO_LPASS_CORE_PERF4_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF4_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF4_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF4_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF4_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF4_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF4_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF4_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF4_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF4_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF4_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF5_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d06c)
#define HWIO_LPASS_CORE_PERF5_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF5_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF5_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF5_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF5_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF5_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF5_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF5_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF5_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF5_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF5_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF6_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d070)
#define HWIO_LPASS_CORE_PERF6_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF6_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF6_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF6_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF6_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF6_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF6_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF6_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF6_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF6_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF6_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF7_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d074)
#define HWIO_LPASS_CORE_PERF7_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF7_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF7_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF7_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF7_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF7_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF7_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF7_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF7_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF7_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF7_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF0_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d078)
#define HWIO_LPASS_CORE_PERF0_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF0_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF0_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF0_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF0_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF0_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF0_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF0_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF0_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF0_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF0_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_CORE_PERF1_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d07c)
#define HWIO_LPASS_CORE_PERF1_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF1_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF1_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF1_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF1_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF1_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF1_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF1_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF1_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF1_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF1_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_CORE_PERF2_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d080)
#define HWIO_LPASS_CORE_PERF2_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF2_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF2_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF2_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF2_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF2_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF2_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF2_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF2_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF2_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF2_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_CORE_PERF3_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d084)
#define HWIO_LPASS_CORE_PERF3_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF3_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF3_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF3_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF3_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF3_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF3_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF3_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF3_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF3_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF3_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_CORE_PERF4_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d088)
#define HWIO_LPASS_CORE_PERF4_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF4_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF4_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF4_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF4_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF4_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF4_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF4_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF4_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF4_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF4_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_CORE_PERF5_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d08c)
#define HWIO_LPASS_CORE_PERF5_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF5_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF5_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF5_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF5_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF5_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF5_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF5_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF5_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF5_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF5_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_CORE_PERF6_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d090)
#define HWIO_LPASS_CORE_PERF6_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF6_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF6_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF6_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF6_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF6_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF6_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF6_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF6_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF6_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF6_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_CORE_PERF7_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d094)
#define HWIO_LPASS_CORE_PERF7_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF7_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF7_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF7_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF7_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF7_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF7_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF7_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF7_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF7_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF7_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_ADDR                                                             (LPASS_CC_REG_BASE      + 0x0001e000)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_RMSK                                                             0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                               0x4000
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                                  0xe
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                             0x2000
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                0xd
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                            0x1000
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                               0xc
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_WAKEUP_BMSK                                                           0xf00
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_WAKEUP_SHFT                                                             0x8
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_SLEEP_BMSK                                                             0xf0
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_SLEEP_SHFT                                                              0x4
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_HW_CTL_BMSK                                                             0x2
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_HW_CTL_SHFT                                                             0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x0001e004)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_RMSK                                                        0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                          0x4000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                             0xe
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                        0x2000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                           0xd
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                       0x1000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                          0xc
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_WAKEUP_BMSK                                                      0xf00
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_WAKEUP_SHFT                                                        0x8
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_SLEEP_BMSK                                                        0xf0
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_SLEEP_SHFT                                                         0x4
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x0001e008)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_RMSK                                                        0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                          0x4000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                             0xe
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                        0x2000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                           0xd
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                       0x1000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                          0xc
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_WAKEUP_BMSK                                                      0xf00
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_WAKEUP_SHFT                                                        0x8
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_SLEEP_BMSK                                                        0xf0
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_SLEEP_SHFT                                                         0x4
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x0001e00c)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_RMSK                                                        0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                          0x4000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                             0xe
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                        0x2000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                           0xd
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                       0x1000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                          0xc
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_WAKEUP_BMSK                                                      0xf00
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_WAKEUP_SHFT                                                        0x8
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_SLEEP_BMSK                                                        0xf0
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_SLEEP_SHFT                                                         0x4
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x0001e010)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_RMSK                                                        0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                          0x4000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                             0xe
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                        0x2000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                           0xd
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                       0x1000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                          0xc
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_WAKEUP_BMSK                                                      0xf00
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_WAKEUP_SHFT                                                        0x8
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_SLEEP_BMSK                                                        0xf0
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_SLEEP_SHFT                                                         0x4
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x0001f000)
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_RMSK                                                                 0x80000003
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_CLK_OFF_BMSK                                                         0x80000000
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_CLK_OFF_SHFT                                                               0x1f
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_HW_CTL_BMSK                                                                 0x2
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_HW_CTL_SHFT                                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_CLK_ENABLE_BMSK                                                             0x1
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_CLK_ENABLE_SHFT                                                             0x0

#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00020000)
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ADDR, HWIO_LPASS_EXT_MCLK0_CMD_RCGR_RMSK)
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ADDR,m,v,HWIO_LPASS_EXT_MCLK0_CMD_RCGR_IN)
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00020004)
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_CFG_RCGR_ADDR, HWIO_LPASS_EXT_MCLK0_CFG_RCGR_RMSK)
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK0_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK0_CFG_RCGR_ADDR,m,v,HWIO_LPASS_EXT_MCLK0_CFG_RCGR_IN)
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_EXT_MCLK0_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00020008)
#define HWIO_LPASS_EXT_MCLK0_M_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK0_M_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_M_ADDR, HWIO_LPASS_EXT_MCLK0_M_RMSK)
#define HWIO_LPASS_EXT_MCLK0_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_M_ADDR, m)
#define HWIO_LPASS_EXT_MCLK0_M_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK0_M_ADDR,v)
#define HWIO_LPASS_EXT_MCLK0_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK0_M_ADDR,m,v,HWIO_LPASS_EXT_MCLK0_M_IN)
#define HWIO_LPASS_EXT_MCLK0_M_M_BMSK                                                                              0xff
#define HWIO_LPASS_EXT_MCLK0_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_EXT_MCLK0_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0002000c)
#define HWIO_LPASS_EXT_MCLK0_N_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK0_N_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_N_ADDR, HWIO_LPASS_EXT_MCLK0_N_RMSK)
#define HWIO_LPASS_EXT_MCLK0_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_N_ADDR, m)
#define HWIO_LPASS_EXT_MCLK0_N_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK0_N_ADDR,v)
#define HWIO_LPASS_EXT_MCLK0_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK0_N_ADDR,m,v,HWIO_LPASS_EXT_MCLK0_N_IN)
#define HWIO_LPASS_EXT_MCLK0_N_NOT_N_MINUS_M_BMSK                                                                  0xff
#define HWIO_LPASS_EXT_MCLK0_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_EXT_MCLK0_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00020010)
#define HWIO_LPASS_EXT_MCLK0_D_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK0_D_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_D_ADDR, HWIO_LPASS_EXT_MCLK0_D_RMSK)
#define HWIO_LPASS_EXT_MCLK0_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_D_ADDR, m)
#define HWIO_LPASS_EXT_MCLK0_D_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK0_D_ADDR,v)
#define HWIO_LPASS_EXT_MCLK0_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK0_D_ADDR,m,v,HWIO_LPASS_EXT_MCLK0_D_IN)
#define HWIO_LPASS_EXT_MCLK0_D_NOT_2D_BMSK                                                                         0xff
#define HWIO_LPASS_EXT_MCLK0_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_ADDR                                                            (LPASS_CC_REG_BASE      + 0x00020014)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_RMSK                                                            0x80000003
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_HW_CTL_BMSK                                                            0x2
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_HW_CTL_SHFT                                                            0x1
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00021000)
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ADDR, HWIO_LPASS_EXT_MCLK1_CMD_RCGR_RMSK)
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ADDR,m,v,HWIO_LPASS_EXT_MCLK1_CMD_RCGR_IN)
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00021004)
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_CFG_RCGR_ADDR, HWIO_LPASS_EXT_MCLK1_CFG_RCGR_RMSK)
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK1_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK1_CFG_RCGR_ADDR,m,v,HWIO_LPASS_EXT_MCLK1_CFG_RCGR_IN)
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_EXT_MCLK1_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00021008)
#define HWIO_LPASS_EXT_MCLK1_M_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK1_M_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_M_ADDR, HWIO_LPASS_EXT_MCLK1_M_RMSK)
#define HWIO_LPASS_EXT_MCLK1_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_M_ADDR, m)
#define HWIO_LPASS_EXT_MCLK1_M_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK1_M_ADDR,v)
#define HWIO_LPASS_EXT_MCLK1_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK1_M_ADDR,m,v,HWIO_LPASS_EXT_MCLK1_M_IN)
#define HWIO_LPASS_EXT_MCLK1_M_M_BMSK                                                                              0xff
#define HWIO_LPASS_EXT_MCLK1_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_EXT_MCLK1_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0002100c)
#define HWIO_LPASS_EXT_MCLK1_N_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK1_N_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_N_ADDR, HWIO_LPASS_EXT_MCLK1_N_RMSK)
#define HWIO_LPASS_EXT_MCLK1_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_N_ADDR, m)
#define HWIO_LPASS_EXT_MCLK1_N_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK1_N_ADDR,v)
#define HWIO_LPASS_EXT_MCLK1_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK1_N_ADDR,m,v,HWIO_LPASS_EXT_MCLK1_N_IN)
#define HWIO_LPASS_EXT_MCLK1_N_NOT_N_MINUS_M_BMSK                                                                  0xff
#define HWIO_LPASS_EXT_MCLK1_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_EXT_MCLK1_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00021010)
#define HWIO_LPASS_EXT_MCLK1_D_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK1_D_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_D_ADDR, HWIO_LPASS_EXT_MCLK1_D_RMSK)
#define HWIO_LPASS_EXT_MCLK1_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_D_ADDR, m)
#define HWIO_LPASS_EXT_MCLK1_D_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK1_D_ADDR,v)
#define HWIO_LPASS_EXT_MCLK1_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK1_D_ADDR,m,v,HWIO_LPASS_EXT_MCLK1_D_IN)
#define HWIO_LPASS_EXT_MCLK1_D_NOT_2D_BMSK                                                                         0xff
#define HWIO_LPASS_EXT_MCLK1_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_ADDR                                                            (LPASS_CC_REG_BASE      + 0x00021014)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_RMSK                                                            0x80000003
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_HW_CTL_BMSK                                                            0x2
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_HW_CTL_SHFT                                                            0x1
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00022000)
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ADDR, HWIO_LPASS_EXT_MCLK2_CMD_RCGR_RMSK)
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ADDR,m,v,HWIO_LPASS_EXT_MCLK2_CMD_RCGR_IN)
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00022004)
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_CFG_RCGR_ADDR, HWIO_LPASS_EXT_MCLK2_CFG_RCGR_RMSK)
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK2_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK2_CFG_RCGR_ADDR,m,v,HWIO_LPASS_EXT_MCLK2_CFG_RCGR_IN)
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_EXT_MCLK2_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00022008)
#define HWIO_LPASS_EXT_MCLK2_M_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK2_M_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_M_ADDR, HWIO_LPASS_EXT_MCLK2_M_RMSK)
#define HWIO_LPASS_EXT_MCLK2_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_M_ADDR, m)
#define HWIO_LPASS_EXT_MCLK2_M_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK2_M_ADDR,v)
#define HWIO_LPASS_EXT_MCLK2_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK2_M_ADDR,m,v,HWIO_LPASS_EXT_MCLK2_M_IN)
#define HWIO_LPASS_EXT_MCLK2_M_M_BMSK                                                                              0xff
#define HWIO_LPASS_EXT_MCLK2_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_EXT_MCLK2_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0002200c)
#define HWIO_LPASS_EXT_MCLK2_N_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK2_N_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_N_ADDR, HWIO_LPASS_EXT_MCLK2_N_RMSK)
#define HWIO_LPASS_EXT_MCLK2_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_N_ADDR, m)
#define HWIO_LPASS_EXT_MCLK2_N_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK2_N_ADDR,v)
#define HWIO_LPASS_EXT_MCLK2_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK2_N_ADDR,m,v,HWIO_LPASS_EXT_MCLK2_N_IN)
#define HWIO_LPASS_EXT_MCLK2_N_NOT_N_MINUS_M_BMSK                                                                  0xff
#define HWIO_LPASS_EXT_MCLK2_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_EXT_MCLK2_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00022010)
#define HWIO_LPASS_EXT_MCLK2_D_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK2_D_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_D_ADDR, HWIO_LPASS_EXT_MCLK2_D_RMSK)
#define HWIO_LPASS_EXT_MCLK2_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_D_ADDR, m)
#define HWIO_LPASS_EXT_MCLK2_D_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK2_D_ADDR,v)
#define HWIO_LPASS_EXT_MCLK2_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK2_D_ADDR,m,v,HWIO_LPASS_EXT_MCLK2_D_IN)
#define HWIO_LPASS_EXT_MCLK2_D_NOT_2D_BMSK                                                                         0xff
#define HWIO_LPASS_EXT_MCLK2_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_ADDR                                                            (LPASS_CC_REG_BASE      + 0x00022014)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_RMSK                                                            0x80000003
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_HW_CTL_BMSK                                                            0x2
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_HW_CTL_SHFT                                                            0x1
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_ADDR                                                    (LPASS_CC_REG_BASE      + 0x00023000)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_RMSK                                                    0x80000003
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_CLK_OFF_BMSK                                            0x80000000
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_CLK_OFF_SHFT                                                  0x1f
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_HW_CTL_BMSK                                                    0x2
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_HW_CTL_SHFT                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_CLK_ENABLE_BMSK                                                0x1
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_CLK_ENABLE_SHFT                                                0x0

#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_ADDR                                                     (LPASS_CC_REG_BASE      + 0x00024000)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_RMSK                                                     0x80000003
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_HW_CTL_BMSK                                                     0x2
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_HW_CTL_SHFT                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_ADDR                                             (LPASS_CC_REG_BASE      + 0x00037008)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_RMSK                                                    0x1
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_IN          \
        in_dword_masked(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_ADDR, HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_RMSK)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_INM(m)      \
        in_dword_masked(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_ADDR, m)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_OUT(v)      \
        out_dword(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_ADDR,v)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_ADDR,m,v,HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_IN)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_LPASS_CORE_SEQUENCE_ABORT_IRQ_EN_BMSK                   0x1
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_LPASS_CORE_SEQUENCE_ABORT_IRQ_EN_SHFT                   0x0

#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_ADDR                                           (LPASS_CC_REG_BASE      + 0x00038008)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_RMSK                                                  0x1
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_IN          \
        in_dword_masked(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_ADDR, HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_RMSK)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_INM(m)      \
        in_dword_masked(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_ADDR, m)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_OUT(v)      \
        out_dword(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_ADDR,v)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_ADDR,m,v,HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_IN)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_LPASS_CORE_SEQUENCE_ABORT_IRQ_EN_BMSK                 0x1
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_LPASS_CORE_SEQUENCE_ABORT_IRQ_EN_SHFT                 0x0

#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_ADDR                                                     (LPASS_CC_REG_BASE      + 0x0003b000)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_RMSK                                                     0x80000003
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_HW_CTL_BMSK                                                     0x2
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_HW_CTL_SHFT                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_JBIST_PLL_SRC_SEL_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0003e000)
#define HWIO_LPASS_JBIST_PLL_SRC_SEL_RMSK                                                                           0x3
#define HWIO_LPASS_JBIST_PLL_SRC_SEL_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_PLL_SRC_SEL_ADDR, HWIO_LPASS_JBIST_PLL_SRC_SEL_RMSK)
#define HWIO_LPASS_JBIST_PLL_SRC_SEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_PLL_SRC_SEL_ADDR, m)
#define HWIO_LPASS_JBIST_PLL_SRC_SEL_OUT(v)      \
        out_dword(HWIO_LPASS_JBIST_PLL_SRC_SEL_ADDR,v)
#define HWIO_LPASS_JBIST_PLL_SRC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_JBIST_PLL_SRC_SEL_ADDR,m,v,HWIO_LPASS_JBIST_PLL_SRC_SEL_IN)
#define HWIO_LPASS_JBIST_PLL_SRC_SEL_JBIST_PLL_CLK_SRC_SEL_BMSK                                                     0x3
#define HWIO_LPASS_JBIST_PLL_SRC_SEL_JBIST_PLL_CLK_SRC_SEL_SHFT                                                     0x0

#define HWIO_LPASS_JBIST_MODE_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x0003e004)
#define HWIO_LPASS_JBIST_MODE_RMSK                                                                           0xffffffff
#define HWIO_LPASS_JBIST_MODE_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_MODE_ADDR, HWIO_LPASS_JBIST_MODE_RMSK)
#define HWIO_LPASS_JBIST_MODE_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_MODE_ADDR, m)
#define HWIO_LPASS_JBIST_MODE_OUT(v)      \
        out_dword(HWIO_LPASS_JBIST_MODE_ADDR,v)
#define HWIO_LPASS_JBIST_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_JBIST_MODE_ADDR,m,v,HWIO_LPASS_JBIST_MODE_IN)
#define HWIO_LPASS_JBIST_MODE_RESERVE_BITS31_4_BMSK                                                          0xfffffff0
#define HWIO_LPASS_JBIST_MODE_RESERVE_BITS31_4_SHFT                                                                 0x4
#define HWIO_LPASS_JBIST_MODE_START_MEAS_BMSK                                                                       0x8
#define HWIO_LPASS_JBIST_MODE_START_MEAS_SHFT                                                                       0x3
#define HWIO_LPASS_JBIST_MODE_JBIST_TEST_BMSK                                                                       0x4
#define HWIO_LPASS_JBIST_MODE_JBIST_TEST_SHFT                                                                       0x2
#define HWIO_LPASS_JBIST_MODE_RESET_N_BMSK                                                                          0x2
#define HWIO_LPASS_JBIST_MODE_RESET_N_SHFT                                                                          0x1
#define HWIO_LPASS_JBIST_MODE_SLEEP_N_BMSK                                                                          0x1
#define HWIO_LPASS_JBIST_MODE_SLEEP_N_SHFT                                                                          0x0

#define HWIO_LPASS_JBIST_CONFIG_CTL_ADDR                                                                     (LPASS_CC_REG_BASE      + 0x0003e008)
#define HWIO_LPASS_JBIST_CONFIG_CTL_RMSK                                                                     0xffffffff
#define HWIO_LPASS_JBIST_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_CONFIG_CTL_ADDR, HWIO_LPASS_JBIST_CONFIG_CTL_RMSK)
#define HWIO_LPASS_JBIST_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_CONFIG_CTL_ADDR, m)
#define HWIO_LPASS_JBIST_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_JBIST_CONFIG_CTL_ADDR,v)
#define HWIO_LPASS_JBIST_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_JBIST_CONFIG_CTL_ADDR,m,v,HWIO_LPASS_JBIST_CONFIG_CTL_IN)
#define HWIO_LPASS_JBIST_CONFIG_CTL_JBIST_CONFIG_CTL_BMSK                                                    0xffffffff
#define HWIO_LPASS_JBIST_CONFIG_CTL_JBIST_CONFIG_CTL_SHFT                                                           0x0

#define HWIO_LPASS_JBIST_USER_CTL_ADDR                                                                       (LPASS_CC_REG_BASE      + 0x0003e00c)
#define HWIO_LPASS_JBIST_USER_CTL_RMSK                                                                       0xffffffff
#define HWIO_LPASS_JBIST_USER_CTL_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_USER_CTL_ADDR, HWIO_LPASS_JBIST_USER_CTL_RMSK)
#define HWIO_LPASS_JBIST_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_USER_CTL_ADDR, m)
#define HWIO_LPASS_JBIST_USER_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_JBIST_USER_CTL_ADDR,v)
#define HWIO_LPASS_JBIST_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_JBIST_USER_CTL_ADDR,m,v,HWIO_LPASS_JBIST_USER_CTL_IN)
#define HWIO_LPASS_JBIST_USER_CTL_JBIST_USER_CTL_BMSK                                                        0xffffffff
#define HWIO_LPASS_JBIST_USER_CTL_JBIST_USER_CTL_SHFT                                                               0x0

#define HWIO_LPASS_JBIST_USER_CTL_U_ADDR                                                                     (LPASS_CC_REG_BASE      + 0x0003e010)
#define HWIO_LPASS_JBIST_USER_CTL_U_RMSK                                                                     0xffffffff
#define HWIO_LPASS_JBIST_USER_CTL_U_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_USER_CTL_U_ADDR, HWIO_LPASS_JBIST_USER_CTL_U_RMSK)
#define HWIO_LPASS_JBIST_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_USER_CTL_U_ADDR, m)
#define HWIO_LPASS_JBIST_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_LPASS_JBIST_USER_CTL_U_ADDR,v)
#define HWIO_LPASS_JBIST_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_JBIST_USER_CTL_U_ADDR,m,v,HWIO_LPASS_JBIST_USER_CTL_U_IN)
#define HWIO_LPASS_JBIST_USER_CTL_U_JBIST_USER_CTL_U_BMSK                                                    0xffffffff
#define HWIO_LPASS_JBIST_USER_CTL_U_JBIST_USER_CTL_U_SHFT                                                           0x0

#define HWIO_LPASS_JBIST_TEST_CTL_ADDR                                                                       (LPASS_CC_REG_BASE      + 0x00003e14)
#define HWIO_LPASS_JBIST_TEST_CTL_RMSK                                                                       0xffffffff
#define HWIO_LPASS_JBIST_TEST_CTL_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_TEST_CTL_ADDR, HWIO_LPASS_JBIST_TEST_CTL_RMSK)
#define HWIO_LPASS_JBIST_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_TEST_CTL_ADDR, m)
#define HWIO_LPASS_JBIST_TEST_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_JBIST_TEST_CTL_ADDR,v)
#define HWIO_LPASS_JBIST_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_JBIST_TEST_CTL_ADDR,m,v,HWIO_LPASS_JBIST_TEST_CTL_IN)
#define HWIO_LPASS_JBIST_TEST_CTL_JBIST_TEST_CTL_BMSK                                                        0xffffffff
#define HWIO_LPASS_JBIST_TEST_CTL_JBIST_TEST_CTL_SHFT                                                               0x0

#define HWIO_LPASS_JBIST_STATUS_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0003e018)
#define HWIO_LPASS_JBIST_STATUS_RMSK                                                                         0xffffffff
#define HWIO_LPASS_JBIST_STATUS_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_STATUS_ADDR, HWIO_LPASS_JBIST_STATUS_RMSK)
#define HWIO_LPASS_JBIST_STATUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_STATUS_ADDR, m)
#define HWIO_LPASS_JBIST_STATUS_JBIST_STATUS_BMSK                                                            0xffffffff
#define HWIO_LPASS_JBIST_STATUS_JBIST_STATUS_SHFT                                                                   0x0

#define HWIO_LPASS_JBIST_MEAS_DONE_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0003e01c)
#define HWIO_LPASS_JBIST_MEAS_DONE_RMSK                                                                      0xffffffff
#define HWIO_LPASS_JBIST_MEAS_DONE_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_MEAS_DONE_ADDR, HWIO_LPASS_JBIST_MEAS_DONE_RMSK)
#define HWIO_LPASS_JBIST_MEAS_DONE_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_MEAS_DONE_ADDR, m)
#define HWIO_LPASS_JBIST_MEAS_DONE_OUT(v)      \
        out_dword(HWIO_LPASS_JBIST_MEAS_DONE_ADDR,v)
#define HWIO_LPASS_JBIST_MEAS_DONE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_JBIST_MEAS_DONE_ADDR,m,v,HWIO_LPASS_JBIST_MEAS_DONE_IN)
#define HWIO_LPASS_JBIST_MEAS_DONE_RESERVE_BITS31_1_BMSK                                                     0xfffffffe
#define HWIO_LPASS_JBIST_MEAS_DONE_RESERVE_BITS31_1_SHFT                                                            0x1
#define HWIO_LPASS_JBIST_MEAS_DONE_JBIST_MEAS_DONE_BMSK                                                             0x1
#define HWIO_LPASS_JBIST_MEAS_DONE_JBIST_MEAS_DONE_SHFT                                                             0x0

#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x00046000)
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_RMSK                                                                  0x800000f3
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ADDR, HWIO_LPASS_INT_I2S0_1_CMD_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S0_1_CMD_RCGR_IN)
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ROOT_OFF_BMSK                                                         0x80000000
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ROOT_OFF_SHFT                                                               0x1f
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_D_BMSK                                                                0x80
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_D_SHFT                                                                 0x7
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_N_BMSK                                                                0x40
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_N_SHFT                                                                 0x6
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_M_BMSK                                                                0x20
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_M_SHFT                                                                 0x5
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                         0x10
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                          0x4
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ROOT_EN_BMSK                                                                 0x2
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ROOT_EN_SHFT                                                                 0x1
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_UPDATE_BMSK                                                                  0x1
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_UPDATE_SHFT                                                                  0x0

#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x00046004)
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_RMSK                                                                      0x771f
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_CFG_RCGR_ADDR, HWIO_LPASS_INT_I2S0_1_CFG_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S0_1_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S0_1_CFG_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S0_1_CFG_RCGR_IN)
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_ALT_SRC_SEL_BMSK                                                          0x4000
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_ALT_SRC_SEL_SHFT                                                             0xe
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_MODE_BMSK                                                                 0x3000
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_MODE_SHFT                                                                    0xc
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_SRC_SEL_BMSK                                                               0x700
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_SRC_SEL_SHFT                                                                 0x8
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_SRC_DIV_BMSK                                                                0x1f
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_SRC_DIV_SHFT                                                                 0x0

#define HWIO_LPASS_INT_I2S0_1_M_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x00046008)
#define HWIO_LPASS_INT_I2S0_1_M_RMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S0_1_M_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_M_ADDR, HWIO_LPASS_INT_I2S0_1_M_RMSK)
#define HWIO_LPASS_INT_I2S0_1_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_M_ADDR, m)
#define HWIO_LPASS_INT_I2S0_1_M_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S0_1_M_ADDR,v)
#define HWIO_LPASS_INT_I2S0_1_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S0_1_M_ADDR,m,v,HWIO_LPASS_INT_I2S0_1_M_IN)
#define HWIO_LPASS_INT_I2S0_1_M_M_BMSK                                                                             0xff
#define HWIO_LPASS_INT_I2S0_1_M_M_SHFT                                                                              0x0

#define HWIO_LPASS_INT_I2S0_1_N_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0004600c)
#define HWIO_LPASS_INT_I2S0_1_N_RMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S0_1_N_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_N_ADDR, HWIO_LPASS_INT_I2S0_1_N_RMSK)
#define HWIO_LPASS_INT_I2S0_1_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_N_ADDR, m)
#define HWIO_LPASS_INT_I2S0_1_N_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S0_1_N_ADDR,v)
#define HWIO_LPASS_INT_I2S0_1_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S0_1_N_ADDR,m,v,HWIO_LPASS_INT_I2S0_1_N_IN)
#define HWIO_LPASS_INT_I2S0_1_N_NOT_N_MINUS_M_BMSK                                                                 0xff
#define HWIO_LPASS_INT_I2S0_1_N_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_LPASS_INT_I2S0_1_D_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x00046010)
#define HWIO_LPASS_INT_I2S0_1_D_RMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S0_1_D_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_D_ADDR, HWIO_LPASS_INT_I2S0_1_D_RMSK)
#define HWIO_LPASS_INT_I2S0_1_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_D_ADDR, m)
#define HWIO_LPASS_INT_I2S0_1_D_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S0_1_D_ADDR,v)
#define HWIO_LPASS_INT_I2S0_1_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S0_1_D_ADDR,m,v,HWIO_LPASS_INT_I2S0_1_D_IN)
#define HWIO_LPASS_INT_I2S0_1_D_NOT_2D_BMSK                                                                        0xff
#define HWIO_LPASS_INT_I2S0_1_D_NOT_2D_SHFT                                                                         0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x00046014)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_RMSK                                                      0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x00046018)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_RMSK                                                      0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_ADDR                                                 (LPASS_CC_REG_BASE      + 0x0004601c)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_RMSK                                                 0x80007f13
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                   0x7f00
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                      0x8
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                             0x10
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                              0x4
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                         0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                         0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_IRQ_EN_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_IRQ_EN_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_ADDR                                                    (LPASS_CC_REG_BASE      + 0x00046020)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_RMSK                                                           0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_TEST_CLK_ENABLE_BMSK                                           0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_TEST_CLK_ENABLE_SHFT                                           0x0

#define HWIO_LPASS_INT_I2S2_CMD_RCGR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x00047000)
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_RMSK                                                                    0x800000f3
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S2_CMD_RCGR_ADDR, HWIO_LPASS_INT_I2S2_CMD_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S2_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S2_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S2_CMD_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S2_CMD_RCGR_IN)
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_ROOT_OFF_BMSK                                                           0x80000000
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_ROOT_OFF_SHFT                                                                 0x1f
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_D_BMSK                                                                  0x80
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_D_SHFT                                                                   0x7
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_N_BMSK                                                                  0x40
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_N_SHFT                                                                   0x6
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_M_BMSK                                                                  0x20
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_M_SHFT                                                                   0x5
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                           0x10
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                            0x4
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_ROOT_EN_BMSK                                                                   0x2
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_ROOT_EN_SHFT                                                                   0x1
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_UPDATE_BMSK                                                                    0x1
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_UPDATE_SHFT                                                                    0x0

#define HWIO_LPASS_INT_I2S2_CFG_RCGR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x00047004)
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_RMSK                                                                        0x771f
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S2_CFG_RCGR_ADDR, HWIO_LPASS_INT_I2S2_CFG_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S2_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S2_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S2_CFG_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S2_CFG_RCGR_IN)
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_ALT_SRC_SEL_BMSK                                                            0x4000
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_ALT_SRC_SEL_SHFT                                                               0xe
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_MODE_BMSK                                                                   0x3000
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_MODE_SHFT                                                                      0xc
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_SRC_SEL_BMSK                                                                 0x700
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_SRC_SEL_SHFT                                                                   0x8
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_SRC_DIV_BMSK                                                                  0x1f
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_SRC_DIV_SHFT                                                                   0x0

#define HWIO_LPASS_INT_I2S2_M_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x00047008)
#define HWIO_LPASS_INT_I2S2_M_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S2_M_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S2_M_ADDR, HWIO_LPASS_INT_I2S2_M_RMSK)
#define HWIO_LPASS_INT_I2S2_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S2_M_ADDR, m)
#define HWIO_LPASS_INT_I2S2_M_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S2_M_ADDR,v)
#define HWIO_LPASS_INT_I2S2_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S2_M_ADDR,m,v,HWIO_LPASS_INT_I2S2_M_IN)
#define HWIO_LPASS_INT_I2S2_M_M_BMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S2_M_M_SHFT                                                                                0x0

#define HWIO_LPASS_INT_I2S2_N_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x0004700c)
#define HWIO_LPASS_INT_I2S2_N_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S2_N_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S2_N_ADDR, HWIO_LPASS_INT_I2S2_N_RMSK)
#define HWIO_LPASS_INT_I2S2_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S2_N_ADDR, m)
#define HWIO_LPASS_INT_I2S2_N_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S2_N_ADDR,v)
#define HWIO_LPASS_INT_I2S2_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S2_N_ADDR,m,v,HWIO_LPASS_INT_I2S2_N_IN)
#define HWIO_LPASS_INT_I2S2_N_NOT_N_MINUS_M_BMSK                                                                   0xff
#define HWIO_LPASS_INT_I2S2_N_NOT_N_MINUS_M_SHFT                                                                    0x0

#define HWIO_LPASS_INT_I2S2_D_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x00047010)
#define HWIO_LPASS_INT_I2S2_D_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S2_D_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S2_D_ADDR, HWIO_LPASS_INT_I2S2_D_RMSK)
#define HWIO_LPASS_INT_I2S2_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S2_D_ADDR, m)
#define HWIO_LPASS_INT_I2S2_D_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S2_D_ADDR,v)
#define HWIO_LPASS_INT_I2S2_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S2_D_ADDR,m,v,HWIO_LPASS_INT_I2S2_D_IN)
#define HWIO_LPASS_INT_I2S2_D_NOT_2D_BMSK                                                                          0xff
#define HWIO_LPASS_INT_I2S2_D_NOT_2D_SHFT                                                                           0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00047014)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_RMSK                                                        0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00047018)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_RMSK                                                        0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_ADDR                                                   (LPASS_CC_REG_BASE      + 0x0004701c)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_RMSK                                                   0x80007f13
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                  0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                        0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                     0x7f00
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                        0x8
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                               0x10
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                                0x4
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                           0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                           0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_IRQ_EN_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_IRQ_EN_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_ADDR                                                      (LPASS_CC_REG_BASE      + 0x00047020)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_RMSK                                                             0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_TEST_CLK_ENABLE_BMSK                                             0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_TEST_CLK_ENABLE_SHFT                                             0x0

#define HWIO_LPASS_INT_I2S3_CMD_RCGR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x00048000)
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_RMSK                                                                    0x800000f3
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S3_CMD_RCGR_ADDR, HWIO_LPASS_INT_I2S3_CMD_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S3_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S3_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S3_CMD_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S3_CMD_RCGR_IN)
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_ROOT_OFF_BMSK                                                           0x80000000
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_ROOT_OFF_SHFT                                                                 0x1f
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_D_BMSK                                                                  0x80
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_D_SHFT                                                                   0x7
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_N_BMSK                                                                  0x40
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_N_SHFT                                                                   0x6
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_M_BMSK                                                                  0x20
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_M_SHFT                                                                   0x5
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                           0x10
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                            0x4
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_ROOT_EN_BMSK                                                                   0x2
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_ROOT_EN_SHFT                                                                   0x1
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_UPDATE_BMSK                                                                    0x1
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_UPDATE_SHFT                                                                    0x0

#define HWIO_LPASS_INT_I2S3_CFG_RCGR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x00048004)
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_RMSK                                                                        0x771f
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S3_CFG_RCGR_ADDR, HWIO_LPASS_INT_I2S3_CFG_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S3_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S3_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S3_CFG_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S3_CFG_RCGR_IN)
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_ALT_SRC_SEL_BMSK                                                            0x4000
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_ALT_SRC_SEL_SHFT                                                               0xe
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_MODE_BMSK                                                                   0x3000
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_MODE_SHFT                                                                      0xc
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_SRC_SEL_BMSK                                                                 0x700
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_SRC_SEL_SHFT                                                                   0x8
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_SRC_DIV_BMSK                                                                  0x1f
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_SRC_DIV_SHFT                                                                   0x0

#define HWIO_LPASS_INT_I2S3_M_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x00048008)
#define HWIO_LPASS_INT_I2S3_M_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S3_M_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S3_M_ADDR, HWIO_LPASS_INT_I2S3_M_RMSK)
#define HWIO_LPASS_INT_I2S3_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S3_M_ADDR, m)
#define HWIO_LPASS_INT_I2S3_M_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S3_M_ADDR,v)
#define HWIO_LPASS_INT_I2S3_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S3_M_ADDR,m,v,HWIO_LPASS_INT_I2S3_M_IN)
#define HWIO_LPASS_INT_I2S3_M_M_BMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S3_M_M_SHFT                                                                                0x0

#define HWIO_LPASS_INT_I2S3_N_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x0004800c)
#define HWIO_LPASS_INT_I2S3_N_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S3_N_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S3_N_ADDR, HWIO_LPASS_INT_I2S3_N_RMSK)
#define HWIO_LPASS_INT_I2S3_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S3_N_ADDR, m)
#define HWIO_LPASS_INT_I2S3_N_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S3_N_ADDR,v)
#define HWIO_LPASS_INT_I2S3_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S3_N_ADDR,m,v,HWIO_LPASS_INT_I2S3_N_IN)
#define HWIO_LPASS_INT_I2S3_N_NOT_N_MINUS_M_BMSK                                                                   0xff
#define HWIO_LPASS_INT_I2S3_N_NOT_N_MINUS_M_SHFT                                                                    0x0

#define HWIO_LPASS_INT_I2S3_D_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x00048010)
#define HWIO_LPASS_INT_I2S3_D_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S3_D_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S3_D_ADDR, HWIO_LPASS_INT_I2S3_D_RMSK)
#define HWIO_LPASS_INT_I2S3_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S3_D_ADDR, m)
#define HWIO_LPASS_INT_I2S3_D_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S3_D_ADDR,v)
#define HWIO_LPASS_INT_I2S3_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S3_D_ADDR,m,v,HWIO_LPASS_INT_I2S3_D_IN)
#define HWIO_LPASS_INT_I2S3_D_NOT_2D_BMSK                                                                          0xff
#define HWIO_LPASS_INT_I2S3_D_NOT_2D_SHFT                                                                           0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00048014)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_RMSK                                                        0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00048018)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_RMSK                                                        0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_ADDR                                                   (LPASS_CC_REG_BASE      + 0x0004801c)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_RMSK                                                   0x80007f13
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                  0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                        0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                     0x7f00
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                        0x8
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                               0x10
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                                0x4
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                           0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                           0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_IRQ_EN_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_IRQ_EN_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_ADDR                                                      (LPASS_CC_REG_BASE      + 0x00048020)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_RMSK                                                             0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_TEST_CLK_ENABLE_BMSK                                             0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_TEST_CLK_ENABLE_SHFT                                             0x0

#define HWIO_LPASS_INT_I2S4_CMD_RCGR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x00049000)
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_RMSK                                                                    0x800000f3
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S4_CMD_RCGR_ADDR, HWIO_LPASS_INT_I2S4_CMD_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S4_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S4_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S4_CMD_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S4_CMD_RCGR_IN)
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_ROOT_OFF_BMSK                                                           0x80000000
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_ROOT_OFF_SHFT                                                                 0x1f
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_D_BMSK                                                                  0x80
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_D_SHFT                                                                   0x7
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_N_BMSK                                                                  0x40
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_N_SHFT                                                                   0x6
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_M_BMSK                                                                  0x20
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_M_SHFT                                                                   0x5
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                           0x10
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                            0x4
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_ROOT_EN_BMSK                                                                   0x2
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_ROOT_EN_SHFT                                                                   0x1
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_UPDATE_BMSK                                                                    0x1
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_UPDATE_SHFT                                                                    0x0

#define HWIO_LPASS_INT_I2S4_CFG_RCGR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x00049004)
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_RMSK                                                                        0x771f
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S4_CFG_RCGR_ADDR, HWIO_LPASS_INT_I2S4_CFG_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S4_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S4_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S4_CFG_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S4_CFG_RCGR_IN)
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_ALT_SRC_SEL_BMSK                                                            0x4000
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_ALT_SRC_SEL_SHFT                                                               0xe
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_MODE_BMSK                                                                   0x3000
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_MODE_SHFT                                                                      0xc
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_SRC_SEL_BMSK                                                                 0x700
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_SRC_SEL_SHFT                                                                   0x8
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_SRC_DIV_BMSK                                                                  0x1f
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_SRC_DIV_SHFT                                                                   0x0

#define HWIO_LPASS_INT_I2S4_M_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x00049008)
#define HWIO_LPASS_INT_I2S4_M_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S4_M_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S4_M_ADDR, HWIO_LPASS_INT_I2S4_M_RMSK)
#define HWIO_LPASS_INT_I2S4_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S4_M_ADDR, m)
#define HWIO_LPASS_INT_I2S4_M_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S4_M_ADDR,v)
#define HWIO_LPASS_INT_I2S4_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S4_M_ADDR,m,v,HWIO_LPASS_INT_I2S4_M_IN)
#define HWIO_LPASS_INT_I2S4_M_M_BMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S4_M_M_SHFT                                                                                0x0

#define HWIO_LPASS_INT_I2S4_N_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x0004900c)
#define HWIO_LPASS_INT_I2S4_N_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S4_N_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S4_N_ADDR, HWIO_LPASS_INT_I2S4_N_RMSK)
#define HWIO_LPASS_INT_I2S4_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S4_N_ADDR, m)
#define HWIO_LPASS_INT_I2S4_N_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S4_N_ADDR,v)
#define HWIO_LPASS_INT_I2S4_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S4_N_ADDR,m,v,HWIO_LPASS_INT_I2S4_N_IN)
#define HWIO_LPASS_INT_I2S4_N_NOT_N_MINUS_M_BMSK                                                                   0xff
#define HWIO_LPASS_INT_I2S4_N_NOT_N_MINUS_M_SHFT                                                                    0x0

#define HWIO_LPASS_INT_I2S4_D_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x00049010)
#define HWIO_LPASS_INT_I2S4_D_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S4_D_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S4_D_ADDR, HWIO_LPASS_INT_I2S4_D_RMSK)
#define HWIO_LPASS_INT_I2S4_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S4_D_ADDR, m)
#define HWIO_LPASS_INT_I2S4_D_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S4_D_ADDR,v)
#define HWIO_LPASS_INT_I2S4_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S4_D_ADDR,m,v,HWIO_LPASS_INT_I2S4_D_IN)
#define HWIO_LPASS_INT_I2S4_D_NOT_2D_BMSK                                                                          0xff
#define HWIO_LPASS_INT_I2S4_D_NOT_2D_SHFT                                                                           0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00049014)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_RMSK                                                        0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00049018)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_RMSK                                                        0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_ADDR                                                   (LPASS_CC_REG_BASE      + 0x0004901c)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_RMSK                                                   0x80007f13
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                  0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                        0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                     0x7f00
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                        0x8
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                               0x10
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                                0x4
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                           0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                           0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_IRQ_EN_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_IRQ_EN_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_ADDR                                                      (LPASS_CC_REG_BASE      + 0x00049020)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_RMSK                                                             0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_TEST_CLK_ENABLE_BMSK                                             0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_TEST_CLK_ENABLE_SHFT                                             0x0

#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x0004a000)
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_RMSK                                                                  0x800000f3
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ADDR, HWIO_LPASS_INT_I2S5_6_CMD_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S5_6_CMD_RCGR_IN)
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ROOT_OFF_BMSK                                                         0x80000000
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ROOT_OFF_SHFT                                                               0x1f
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_D_BMSK                                                                0x80
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_D_SHFT                                                                 0x7
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_N_BMSK                                                                0x40
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_N_SHFT                                                                 0x6
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_M_BMSK                                                                0x20
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_M_SHFT                                                                 0x5
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                         0x10
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                          0x4
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ROOT_EN_BMSK                                                                 0x2
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ROOT_EN_SHFT                                                                 0x1
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_UPDATE_BMSK                                                                  0x1
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_UPDATE_SHFT                                                                  0x0

#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x0004a004)
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_RMSK                                                                      0x771f
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_CFG_RCGR_ADDR, HWIO_LPASS_INT_I2S5_6_CFG_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S5_6_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S5_6_CFG_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S5_6_CFG_RCGR_IN)
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_ALT_SRC_SEL_BMSK                                                          0x4000
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_ALT_SRC_SEL_SHFT                                                             0xe
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_MODE_BMSK                                                                 0x3000
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_MODE_SHFT                                                                    0xc
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_SRC_SEL_BMSK                                                               0x700
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_SRC_SEL_SHFT                                                                 0x8
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_SRC_DIV_BMSK                                                                0x1f
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_SRC_DIV_SHFT                                                                 0x0

#define HWIO_LPASS_INT_I2S5_6_M_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0004a008)
#define HWIO_LPASS_INT_I2S5_6_M_RMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S5_6_M_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_M_ADDR, HWIO_LPASS_INT_I2S5_6_M_RMSK)
#define HWIO_LPASS_INT_I2S5_6_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_M_ADDR, m)
#define HWIO_LPASS_INT_I2S5_6_M_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S5_6_M_ADDR,v)
#define HWIO_LPASS_INT_I2S5_6_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S5_6_M_ADDR,m,v,HWIO_LPASS_INT_I2S5_6_M_IN)
#define HWIO_LPASS_INT_I2S5_6_M_M_BMSK                                                                             0xff
#define HWIO_LPASS_INT_I2S5_6_M_M_SHFT                                                                              0x0

#define HWIO_LPASS_INT_I2S5_6_N_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0004a00c)
#define HWIO_LPASS_INT_I2S5_6_N_RMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S5_6_N_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_N_ADDR, HWIO_LPASS_INT_I2S5_6_N_RMSK)
#define HWIO_LPASS_INT_I2S5_6_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_N_ADDR, m)
#define HWIO_LPASS_INT_I2S5_6_N_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S5_6_N_ADDR,v)
#define HWIO_LPASS_INT_I2S5_6_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S5_6_N_ADDR,m,v,HWIO_LPASS_INT_I2S5_6_N_IN)
#define HWIO_LPASS_INT_I2S5_6_N_NOT_N_MINUS_M_BMSK                                                                 0xff
#define HWIO_LPASS_INT_I2S5_6_N_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_LPASS_INT_I2S5_6_D_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0004a010)
#define HWIO_LPASS_INT_I2S5_6_D_RMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S5_6_D_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_D_ADDR, HWIO_LPASS_INT_I2S5_6_D_RMSK)
#define HWIO_LPASS_INT_I2S5_6_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_D_ADDR, m)
#define HWIO_LPASS_INT_I2S5_6_D_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S5_6_D_ADDR,v)
#define HWIO_LPASS_INT_I2S5_6_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S5_6_D_ADDR,m,v,HWIO_LPASS_INT_I2S5_6_D_IN)
#define HWIO_LPASS_INT_I2S5_6_D_NOT_2D_BMSK                                                                        0xff
#define HWIO_LPASS_INT_I2S5_6_D_NOT_2D_SHFT                                                                         0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x0004a014)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_RMSK                                                      0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x0004a018)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_RMSK                                                      0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_ADDR                                                 (LPASS_CC_REG_BASE      + 0x0004a01c)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_RMSK                                                 0x80007f13
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                   0x7f00
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                      0x8
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                             0x10
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                              0x4
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                         0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                         0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_IRQ_EN_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_IRQ_EN_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_ADDR                                                    (LPASS_CC_REG_BASE      + 0x0004a020)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_RMSK                                                           0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_TEST_CLK_ENABLE_BMSK                                           0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_TEST_CLK_ENABLE_SHFT                                           0x0

#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x0004b000)
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_CMD_RCGR_ADDR, HWIO_LPASS_INT_MCLK0_CMD_RCGR_RMSK)
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK0_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK0_CMD_RCGR_ADDR,m,v,HWIO_LPASS_INT_MCLK0_CMD_RCGR_IN)
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x0004b004)
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_CFG_RCGR_ADDR, HWIO_LPASS_INT_MCLK0_CFG_RCGR_RMSK)
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK0_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK0_CFG_RCGR_ADDR,m,v,HWIO_LPASS_INT_MCLK0_CFG_RCGR_IN)
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_INT_MCLK0_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0004b008)
#define HWIO_LPASS_INT_MCLK0_M_RMSK                                                                                0xff
#define HWIO_LPASS_INT_MCLK0_M_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_M_ADDR, HWIO_LPASS_INT_MCLK0_M_RMSK)
#define HWIO_LPASS_INT_MCLK0_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_M_ADDR, m)
#define HWIO_LPASS_INT_MCLK0_M_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK0_M_ADDR,v)
#define HWIO_LPASS_INT_MCLK0_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK0_M_ADDR,m,v,HWIO_LPASS_INT_MCLK0_M_IN)
#define HWIO_LPASS_INT_MCLK0_M_M_BMSK                                                                              0xff
#define HWIO_LPASS_INT_MCLK0_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_INT_MCLK0_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0004b00c)
#define HWIO_LPASS_INT_MCLK0_N_RMSK                                                                                0xff
#define HWIO_LPASS_INT_MCLK0_N_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_N_ADDR, HWIO_LPASS_INT_MCLK0_N_RMSK)
#define HWIO_LPASS_INT_MCLK0_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_N_ADDR, m)
#define HWIO_LPASS_INT_MCLK0_N_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK0_N_ADDR,v)
#define HWIO_LPASS_INT_MCLK0_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK0_N_ADDR,m,v,HWIO_LPASS_INT_MCLK0_N_IN)
#define HWIO_LPASS_INT_MCLK0_N_NOT_N_MINUS_M_BMSK                                                                  0xff
#define HWIO_LPASS_INT_MCLK0_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_INT_MCLK0_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0004b010)
#define HWIO_LPASS_INT_MCLK0_D_RMSK                                                                                0xff
#define HWIO_LPASS_INT_MCLK0_D_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_D_ADDR, HWIO_LPASS_INT_MCLK0_D_RMSK)
#define HWIO_LPASS_INT_MCLK0_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_D_ADDR, m)
#define HWIO_LPASS_INT_MCLK0_D_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK0_D_ADDR,v)
#define HWIO_LPASS_INT_MCLK0_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK0_D_ADDR,m,v,HWIO_LPASS_INT_MCLK0_D_IN)
#define HWIO_LPASS_INT_MCLK0_D_NOT_2D_BMSK                                                                         0xff
#define HWIO_LPASS_INT_MCLK0_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_ADDR                                                            (LPASS_CC_REG_BASE      + 0x0004b014)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_RMSK                                                            0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_HW_CTL_BMSK                                                            0x2
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_HW_CTL_SHFT                                                            0x1
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x0004c000)
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_CMD_RCGR_ADDR, HWIO_LPASS_INT_MCLK1_CMD_RCGR_RMSK)
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK1_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK1_CMD_RCGR_ADDR,m,v,HWIO_LPASS_INT_MCLK1_CMD_RCGR_IN)
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x0004c004)
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_CFG_RCGR_ADDR, HWIO_LPASS_INT_MCLK1_CFG_RCGR_RMSK)
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK1_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK1_CFG_RCGR_ADDR,m,v,HWIO_LPASS_INT_MCLK1_CFG_RCGR_IN)
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_INT_MCLK1_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0004c008)
#define HWIO_LPASS_INT_MCLK1_M_RMSK                                                                                0xff
#define HWIO_LPASS_INT_MCLK1_M_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_M_ADDR, HWIO_LPASS_INT_MCLK1_M_RMSK)
#define HWIO_LPASS_INT_MCLK1_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_M_ADDR, m)
#define HWIO_LPASS_INT_MCLK1_M_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK1_M_ADDR,v)
#define HWIO_LPASS_INT_MCLK1_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK1_M_ADDR,m,v,HWIO_LPASS_INT_MCLK1_M_IN)
#define HWIO_LPASS_INT_MCLK1_M_M_BMSK                                                                              0xff
#define HWIO_LPASS_INT_MCLK1_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_INT_MCLK1_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0004c00c)
#define HWIO_LPASS_INT_MCLK1_N_RMSK                                                                                0xff
#define HWIO_LPASS_INT_MCLK1_N_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_N_ADDR, HWIO_LPASS_INT_MCLK1_N_RMSK)
#define HWIO_LPASS_INT_MCLK1_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_N_ADDR, m)
#define HWIO_LPASS_INT_MCLK1_N_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK1_N_ADDR,v)
#define HWIO_LPASS_INT_MCLK1_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK1_N_ADDR,m,v,HWIO_LPASS_INT_MCLK1_N_IN)
#define HWIO_LPASS_INT_MCLK1_N_NOT_N_MINUS_M_BMSK                                                                  0xff
#define HWIO_LPASS_INT_MCLK1_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_INT_MCLK1_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0004c010)
#define HWIO_LPASS_INT_MCLK1_D_RMSK                                                                                0xff
#define HWIO_LPASS_INT_MCLK1_D_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_D_ADDR, HWIO_LPASS_INT_MCLK1_D_RMSK)
#define HWIO_LPASS_INT_MCLK1_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_D_ADDR, m)
#define HWIO_LPASS_INT_MCLK1_D_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK1_D_ADDR,v)
#define HWIO_LPASS_INT_MCLK1_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK1_D_ADDR,m,v,HWIO_LPASS_INT_MCLK1_D_IN)
#define HWIO_LPASS_INT_MCLK1_D_NOT_2D_BMSK                                                                         0xff
#define HWIO_LPASS_INT_MCLK1_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_ADDR                                                            (LPASS_CC_REG_BASE      + 0x0004c014)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_RMSK                                                            0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_HW_CTL_BMSK                                                            0x2
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_HW_CTL_SHFT                                                            0x1
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_LPASS_SW_NPL_CMD_RCGR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0004d000)
#define HWIO_LPASS_SW_NPL_CMD_RCGR_RMSK                                                                      0x800000f3
#define HWIO_LPASS_SW_NPL_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_SW_NPL_CMD_RCGR_ADDR, HWIO_LPASS_SW_NPL_CMD_RCGR_RMSK)
#define HWIO_LPASS_SW_NPL_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_SW_NPL_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_SW_NPL_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_SW_NPL_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_SW_NPL_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SW_NPL_CMD_RCGR_ADDR,m,v,HWIO_LPASS_SW_NPL_CMD_RCGR_IN)
#define HWIO_LPASS_SW_NPL_CMD_RCGR_ROOT_OFF_BMSK                                                             0x80000000
#define HWIO_LPASS_SW_NPL_CMD_RCGR_ROOT_OFF_SHFT                                                                   0x1f
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_D_BMSK                                                                    0x80
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_D_SHFT                                                                     0x7
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_N_BMSK                                                                    0x40
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_N_SHFT                                                                     0x6
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_M_BMSK                                                                    0x20
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_M_SHFT                                                                     0x5
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                             0x10
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                              0x4
#define HWIO_LPASS_SW_NPL_CMD_RCGR_ROOT_EN_BMSK                                                                     0x2
#define HWIO_LPASS_SW_NPL_CMD_RCGR_ROOT_EN_SHFT                                                                     0x1
#define HWIO_LPASS_SW_NPL_CMD_RCGR_UPDATE_BMSK                                                                      0x1
#define HWIO_LPASS_SW_NPL_CMD_RCGR_UPDATE_SHFT                                                                      0x0

#define HWIO_LPASS_SW_NPL_CFG_RCGR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0004d004)
#define HWIO_LPASS_SW_NPL_CFG_RCGR_RMSK                                                                          0x771f
#define HWIO_LPASS_SW_NPL_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_SW_NPL_CFG_RCGR_ADDR, HWIO_LPASS_SW_NPL_CFG_RCGR_RMSK)
#define HWIO_LPASS_SW_NPL_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_SW_NPL_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_SW_NPL_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_SW_NPL_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_SW_NPL_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SW_NPL_CFG_RCGR_ADDR,m,v,HWIO_LPASS_SW_NPL_CFG_RCGR_IN)
#define HWIO_LPASS_SW_NPL_CFG_RCGR_ALT_SRC_SEL_BMSK                                                              0x4000
#define HWIO_LPASS_SW_NPL_CFG_RCGR_ALT_SRC_SEL_SHFT                                                                 0xe
#define HWIO_LPASS_SW_NPL_CFG_RCGR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_SW_NPL_CFG_RCGR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_SW_NPL_CFG_RCGR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_SW_NPL_CFG_RCGR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_SW_NPL_CFG_RCGR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_SW_NPL_CFG_RCGR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_SW_NPL_M_ADDR                                                                             (LPASS_CC_REG_BASE      + 0x0004d008)
#define HWIO_LPASS_SW_NPL_M_RMSK                                                                                   0xff
#define HWIO_LPASS_SW_NPL_M_IN          \
        in_dword_masked(HWIO_LPASS_SW_NPL_M_ADDR, HWIO_LPASS_SW_NPL_M_RMSK)
#define HWIO_LPASS_SW_NPL_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_SW_NPL_M_ADDR, m)
#define HWIO_LPASS_SW_NPL_M_OUT(v)      \
        out_dword(HWIO_LPASS_SW_NPL_M_ADDR,v)
#define HWIO_LPASS_SW_NPL_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SW_NPL_M_ADDR,m,v,HWIO_LPASS_SW_NPL_M_IN)
#define HWIO_LPASS_SW_NPL_M_M_BMSK                                                                                 0xff
#define HWIO_LPASS_SW_NPL_M_M_SHFT                                                                                  0x0

#define HWIO_LPASS_SW_NPL_N_ADDR                                                                             (LPASS_CC_REG_BASE      + 0x0004d00c)
#define HWIO_LPASS_SW_NPL_N_RMSK                                                                                   0xff
#define HWIO_LPASS_SW_NPL_N_IN          \
        in_dword_masked(HWIO_LPASS_SW_NPL_N_ADDR, HWIO_LPASS_SW_NPL_N_RMSK)
#define HWIO_LPASS_SW_NPL_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_SW_NPL_N_ADDR, m)
#define HWIO_LPASS_SW_NPL_N_OUT(v)      \
        out_dword(HWIO_LPASS_SW_NPL_N_ADDR,v)
#define HWIO_LPASS_SW_NPL_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SW_NPL_N_ADDR,m,v,HWIO_LPASS_SW_NPL_N_IN)
#define HWIO_LPASS_SW_NPL_N_NOT_N_MINUS_M_BMSK                                                                     0xff
#define HWIO_LPASS_SW_NPL_N_NOT_N_MINUS_M_SHFT                                                                      0x0

#define HWIO_LPASS_SW_NPL_D_ADDR                                                                             (LPASS_CC_REG_BASE      + 0x0004d010)
#define HWIO_LPASS_SW_NPL_D_RMSK                                                                                   0xff
#define HWIO_LPASS_SW_NPL_D_IN          \
        in_dword_masked(HWIO_LPASS_SW_NPL_D_ADDR, HWIO_LPASS_SW_NPL_D_RMSK)
#define HWIO_LPASS_SW_NPL_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_SW_NPL_D_ADDR, m)
#define HWIO_LPASS_SW_NPL_D_OUT(v)      \
        out_dword(HWIO_LPASS_SW_NPL_D_ADDR,v)
#define HWIO_LPASS_SW_NPL_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SW_NPL_D_ADDR,m,v,HWIO_LPASS_SW_NPL_D_IN)
#define HWIO_LPASS_SW_NPL_D_NOT_2D_BMSK                                                                            0xff
#define HWIO_LPASS_SW_NPL_D_NOT_2D_SHFT                                                                             0x0

#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0004d014)
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_RMSK                                                               0x80000003
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_CLK_OFF_BMSK                                                       0x80000000
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_CLK_OFF_SHFT                                                             0x1f
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_HW_CTL_BMSK                                                               0x2
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_HW_CTL_SHFT                                                               0x1
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_CLK_ENABLE_BMSK                                                           0x1
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_CLK_ENABLE_SHFT                                                           0x0

#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_ADDR                                                              (LPASS_CC_REG_BASE      + 0x0004e000)
#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_RMSK                                                                  0xffff
#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_ADDR, HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_RMSK)
#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_ADDR,m,v,HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_IN)
#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_MUX_SEL_BMSK                                                          0xffff
#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_MUX_SEL_SHFT                                                             0x0

#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_ADDR                                                             (LPASS_CC_REG_BASE      + 0x0004e004)
#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_RMSK                                                                    0x3
#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_ADDR, HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_RMSK)
#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_ADDR,m,v,HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_IN)
#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_CLK_DIV_BMSK                                                            0x3
#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_CLK_DIV_SHFT                                                            0x0

#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x0004e008)
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_RMSK                                                                  0x80000001
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_DEBUG_CBCR_ADDR, HWIO_LPASS_LPASS_CC_DEBUG_CBCR_RMSK)
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_DEBUG_CBCR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_DEBUG_CBCR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_DEBUG_CBCR_ADDR,m,v,HWIO_LPASS_LPASS_CC_DEBUG_CBCR_IN)
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_CLK_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_CLK_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_CLK_ENABLE_BMSK                                                              0x1
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_CLK_ENABLE_SHFT                                                              0x0

#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_ADDR                                                           (LPASS_CC_REG_BASE      + 0x0004e00c)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_RMSK                                                                  0x3
#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_ADDR, HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_RMSK)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_ADDR,m,v,HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_IN)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_MUX_SEL_BMSK                                                          0x3
#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_MUX_SEL_SHFT                                                          0x0

#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_ADDR                                                          (LPASS_CC_REG_BASE      + 0x0004e010)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_RMSK                                                                 0x3
#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_ADDR, HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_RMSK)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_ADDR,m,v,HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_IN)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_CLK_DIV_BMSK                                                         0x3
#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_CLK_DIV_SHFT                                                         0x0

#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0004e014)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_RMSK                                                               0x80000001
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_ADDR, HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_RMSK)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_ADDR,m,v,HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_IN)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_CLK_OFF_BMSK                                                       0x80000000
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_CLK_OFF_SHFT                                                             0x1f
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_CLK_ENABLE_BMSK                                                           0x1
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_CLK_ENABLE_SHFT                                                           0x0

#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_ADDR                                                            (LPASS_CC_REG_BASE      + 0x0004e018)
#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_RMSK                                                                   0x7
#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_ADDR, HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_RMSK)
#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_ADDR,m,v,HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_IN)
#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_MUX_SEL_BMSK                                                           0x7
#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_MUX_SEL_SHFT                                                           0x0

#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_ADDR                                                           (LPASS_CC_REG_BASE      + 0x0004e020)
#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_RMSK                                                                  0x7
#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_ADDR, HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_RMSK)
#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_ADDR,m,v,HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_IN)
#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_MUX_SEL_BMSK                                                          0x7
#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_MUX_SEL_SHFT                                                          0x0

#define HWIO_LPASS_TEST_BUS_SEL_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0004e024)
#define HWIO_LPASS_TEST_BUS_SEL_RMSK                                                                                0x3
#define HWIO_LPASS_TEST_BUS_SEL_IN          \
        in_dword_masked(HWIO_LPASS_TEST_BUS_SEL_ADDR, HWIO_LPASS_TEST_BUS_SEL_RMSK)
#define HWIO_LPASS_TEST_BUS_SEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_TEST_BUS_SEL_ADDR, m)
#define HWIO_LPASS_TEST_BUS_SEL_OUT(v)      \
        out_dword(HWIO_LPASS_TEST_BUS_SEL_ADDR,v)
#define HWIO_LPASS_TEST_BUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_TEST_BUS_SEL_ADDR,m,v,HWIO_LPASS_TEST_BUS_SEL_IN)
#define HWIO_LPASS_TEST_BUS_SEL_SEL_BMSK                                                                            0x3
#define HWIO_LPASS_TEST_BUS_SEL_SEL_SHFT                                                                            0x0

#define HWIO_LPASS_LPASS_CC_SPARE_REG_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x0004f000)
#define HWIO_LPASS_LPASS_CC_SPARE_REG_RMSK                                                                   0xffffffff
#define HWIO_LPASS_LPASS_CC_SPARE_REG_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_SPARE_REG_ADDR, HWIO_LPASS_LPASS_CC_SPARE_REG_RMSK)
#define HWIO_LPASS_LPASS_CC_SPARE_REG_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_SPARE_REG_ADDR, m)
#define HWIO_LPASS_LPASS_CC_SPARE_REG_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_SPARE_REG_ADDR,v)
#define HWIO_LPASS_LPASS_CC_SPARE_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_SPARE_REG_ADDR,m,v,HWIO_LPASS_LPASS_CC_SPARE_REG_IN)
#define HWIO_LPASS_LPASS_CC_SPARE_REG_SPARE_BMSK                                                             0xffffffff
#define HWIO_LPASS_LPASS_CC_SPARE_REG_SPARE_SHFT                                                                    0x0

#define HWIO_LPASS_LPASS_CC_SPARE1_REG_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x0004f004)
#define HWIO_LPASS_LPASS_CC_SPARE1_REG_RMSK                                                                  0xffffffff
#define HWIO_LPASS_LPASS_CC_SPARE1_REG_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_SPARE1_REG_ADDR, HWIO_LPASS_LPASS_CC_SPARE1_REG_RMSK)
#define HWIO_LPASS_LPASS_CC_SPARE1_REG_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_SPARE1_REG_ADDR, m)
#define HWIO_LPASS_LPASS_CC_SPARE1_REG_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_SPARE1_REG_ADDR,v)
#define HWIO_LPASS_LPASS_CC_SPARE1_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_SPARE1_REG_ADDR,m,v,HWIO_LPASS_LPASS_CC_SPARE1_REG_IN)
#define HWIO_LPASS_LPASS_CC_SPARE1_REG_SPARE1_BMSK                                                           0xffffffff
#define HWIO_LPASS_LPASS_CC_SPARE1_REG_SPARE1_SHFT                                                                  0x0

/*----------------------------------------------------------------------------
 * MODULE: TURING_CC
 *--------------------------------------------------------------------------*/

#define TURING_CC_REG_BASE                                                                                        (TURING_BASE      + 0x00000000)

#define HWIO_TURING_TURING_DIG_PLL_L_VAL_ADDR                                                                     (TURING_CC_REG_BASE      + 0x00001004)
#define HWIO_TURING_TURING_DIG_PLL_L_VAL_RMSK                                                                         0xffff
#define HWIO_TURING_TURING_DIG_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_L_VAL_ADDR, HWIO_TURING_TURING_DIG_PLL_L_VAL_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_L_VAL_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_L_VAL_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_L_VAL_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_L_VAL_IN)
#define HWIO_TURING_TURING_DIG_PLL_L_VAL_PLL_L_BMSK                                                                   0xffff
#define HWIO_TURING_TURING_DIG_PLL_L_VAL_PLL_L_SHFT                                                                      0x0

#define HWIO_TURING_TURING_DIG_PLL_CAL_L_VAL_ADDR                                                                 (TURING_CC_REG_BASE      + 0x00001008)
#define HWIO_TURING_TURING_DIG_PLL_CAL_L_VAL_RMSK                                                                     0xffff
#define HWIO_TURING_TURING_DIG_PLL_CAL_L_VAL_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_CAL_L_VAL_ADDR, HWIO_TURING_TURING_DIG_PLL_CAL_L_VAL_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_CAL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_CAL_L_VAL_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_CAL_L_VAL_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_CAL_L_VAL_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_CAL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_CAL_L_VAL_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_CAL_L_VAL_IN)
#define HWIO_TURING_TURING_DIG_PLL_CAL_L_VAL_PLL_CAL_L_BMSK                                                           0xffff
#define HWIO_TURING_TURING_DIG_PLL_CAL_L_VAL_PLL_CAL_L_SHFT                                                              0x0

#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_ADDR                                                                  (TURING_CC_REG_BASE      + 0x0000100c)
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_RMSK                                                                  0xffffffff
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_USER_CTL_ADDR, HWIO_TURING_TURING_DIG_PLL_USER_CTL_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_USER_CTL_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_USER_CTL_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_USER_CTL_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_USER_CTL_IN)
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_RESERVE_BITS31_19_BMSK                                                0xfff80000
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_RESERVE_BITS31_19_SHFT                                                      0x13
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                                                       0x70000
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                                                          0x10
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_POST_DIV_RATIO_ODD_BMSK                                                   0xf000
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_POST_DIV_RATIO_ODD_SHFT                                                      0xc
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_POST_DIV_RATIO_EVEN_BMSK                                                   0xf00
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_POST_DIV_RATIO_EVEN_SHFT                                                     0x8
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_OUT_CLK_POLARITY_BMSK                                                       0x80
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_OUT_CLK_POLARITY_SHFT                                                        0x7
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_RESERVE_BITS6_5_BMSK                                                        0x60
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_RESERVE_BITS6_5_SHFT                                                         0x5
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_PLLOUT_TEST_BMSK                                                            0x10
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_PLLOUT_TEST_SHFT                                                             0x4
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_RESERVE_BITS3_BMSK                                                           0x8
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_RESERVE_BITS3_SHFT                                                           0x3
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_PLLOUT_ODD_BMSK                                                              0x4
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_PLLOUT_ODD_SHFT                                                              0x2
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_PLLOUT_EVEN_BMSK                                                             0x2
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_PLLOUT_EVEN_SHFT                                                             0x1
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_PLLOUT_MAIN_BMSK                                                             0x1
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_PLLOUT_MAIN_SHFT                                                             0x0

#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_ADDR                                                                (TURING_CC_REG_BASE      + 0x00001010)
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_RMSK                                                                0xffffffff
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_ADDR, HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_IN)
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_RESERVE_BITS31_22_BMSK                                              0xffc00000
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_RESERVE_BITS31_22_SHFT                                                    0x16
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_FREEZE_LOCK_DETECTOR_IN_DROOP_BMSK                                    0x200000
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_FREEZE_LOCK_DETECTOR_IN_DROOP_SHFT                                        0x15
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_DROOP_DIVIDER_SEL_BMSK                                                0x100000
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_DROOP_DIVIDER_SEL_SHFT                                                    0x14
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_SCALE_FREQ_ON_DROOP_BMSK                                               0x80000
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_SCALE_FREQ_ON_DROOP_SHFT                                                  0x13
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_BMSK                                               0x40000
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_SHFT                                                  0x12
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_BMSK                                               0x20000
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_SHFT                                                  0x11
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_LOW_JITTER_MODE_EN_BMSK                                                0x10000
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_LOW_JITTER_MODE_EN_SHFT                                                   0x10
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_FRAC_FORMAT_SEL_BMSK                                                    0x8000
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_FRAC_FORMAT_SEL_SHFT                                                       0xf
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_BIAS_ON_IN_STANDBY_BMSK                                                 0x4000
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_BIAS_ON_IN_STANDBY_SHFT                                                    0xe
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_REF_CLK_AT_OUT_BMSK                                                     0x2000
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_REF_CLK_AT_OUT_SHFT                                                        0xd
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_SCALE_FREQ_ON_RESTART_BMSK                                              0x1800
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_SCALE_FREQ_ON_RESTART_SHFT                                                 0xb
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_BMSK                                              0x400
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_SHFT                                                0xa
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                                                     0x3e0
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                                                       0x5
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_WRITE_STATE_EN_BMSK                                                       0x10
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_WRITE_STATE_EN_SHFT                                                        0x4
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_CALIB_CTRL_BMSK                                                            0xe
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_CALIB_CTRL_SHFT                                                            0x1
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_BMSK                                                0x1
#define HWIO_TURING_TURING_DIG_PLL_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_SHFT                                                0x0

#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_ADDR                                                                (TURING_CC_REG_BASE      + 0x00001014)
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_RMSK                                                                0xffffffff
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_ADDR, HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_IN)
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_FINE_LDC_THRESHOLD_BMSK                                             0xfc000000
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_FINE_LDC_THRESHOLD_SHFT                                                   0x1a
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_COARSE_LDC_THRESHOLD_BMSK                                            0x3c00000
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_COARSE_LDC_THRESHOLD_SHFT                                                 0x16
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_BMSK                                    0x3c0000
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_SHFT                                        0x12
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_BMSK                                     0x3c000
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_SHFT                                         0xe
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_BMSK                                            0x3800
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_SHFT                                               0xb
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_BMSK                                           0x700
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_SHFT                                             0x8
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_BMSK                                                 0xf0
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_SHFT                                                  0x4
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_FWD_GAIN_KFN_BMSK                                                          0xf
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_FWD_GAIN_KFN_SHFT                                                          0x0

#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_ADDR                                                              (TURING_CC_REG_BASE      + 0x00001018)
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_RMSK                                                              0xffffffff
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_ADDR, HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_IN)
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_BIST_CFG_BMSK                                                     0xfff00000
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_BIST_CFG_SHFT                                                           0x14
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_RESERVE_BITS_19_18_BMSK                                              0xc0000
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_RESERVE_BITS_19_18_SHFT                                                 0x12
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_FORCE_DAC_ON_BMSK                                                    0x30000
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_FORCE_DAC_ON_SHFT                                                       0x10
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_DIS_DMET_DROOP_DIV_BMSK                                               0x8000
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_DIS_DMET_DROOP_DIV_SHFT                                                  0xf
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_BYPASS_DROOP_DIV_BMSK                                                 0x4000
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_BYPASS_DROOP_DIV_SHFT                                                    0xe
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_OSC_SETTLING_TIME_BMSK                                                0x3000
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_OSC_SETTLING_TIME_SHFT                                                   0xc
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_BMSK                                               0xc00
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_SHFT                                                 0xa
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_PREDIV_WHEN_CAL_BMSK                                                   0x380
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_PREDIV_WHEN_CAL_SHFT                                                     0x7
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_BMSK                                        0x40
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_SHFT                                         0x6
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_BMSK                                               0x30
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_SHFT                                                0x4
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_BMSK                                               0x8
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_SHFT                                               0x3
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_BIAS_WARMUP_TIME_BMSK                                                    0x6
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_BIAS_WARMUP_TIME_SHFT                                                    0x1
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_BMSK                                             0x1
#define HWIO_TURING_TURING_DIG_PLL_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_SHFT                                             0x0

#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_ADDR                                                                  (TURING_CC_REG_BASE      + 0x0000101c)
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_RMSK                                                                  0xffffffff
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_TEST_CTL_ADDR, HWIO_TURING_TURING_DIG_PLL_TEST_CTL_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_TEST_CTL_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_TEST_CTL_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_TEST_CTL_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_TEST_CTL_IN)
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_BMSK                                          0xc0000000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_SHFT                                                0x1e
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_BMSK                                      0x30000000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_SHFT                                            0x1c
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_GLITCH_PREVENTION_DIS_BMSK                                             0x8000000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_GLITCH_PREVENTION_DIS_SHFT                                                  0x1b
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_FINE_FCW_BMSK                                                          0x7e00000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_FINE_FCW_SHFT                                                               0x15
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_OVERRIDE_FINE_FCW_BMSK                                                  0x100000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_OVERRIDE_FINE_FCW_SHFT                                                      0x14
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_COARSE_FCW_BMSK                                                          0xff000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_COARSE_FCW_SHFT                                                              0xc
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_OVERRIDE_COARSE_FCW_BMSK                                                   0x800
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_OVERRIDE_COARSE_FCW_SHFT                                                     0xb
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_PROCESS_CALIB_WORD_BMSK                                                    0x700
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_PROCESS_CALIB_WORD_SHFT                                                      0x8
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_OVERRIDE_CALIB_WORD_BMSK                                                    0x80
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_OVERRIDE_CALIB_WORD_SHFT                                                     0x7
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_DISABLE_LFSR_BMSK                                                           0x40
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_DISABLE_LFSR_SHFT                                                            0x6
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_EN_VREF_TEST_BMSK                                                           0x20
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_EN_VREF_TEST_SHFT                                                            0x5
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_EN_VDCO_TEST_BMSK                                                           0x10
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_EN_VDCO_TEST_SHFT                                                            0x4
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_BYPASS_ATEST_BUFFER_BMSK                                                     0x8
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_BYPASS_ATEST_BUFFER_SHFT                                                     0x3
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_EN_IVCO_TEST_BMSK                                                            0x4
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_EN_IVCO_TEST_SHFT                                                            0x2
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_ATEST1_EN_BMSK                                                               0x2
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_ATEST1_EN_SHFT                                                               0x1
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_ATEST0_EN_BMSK                                                               0x1
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_ATEST0_EN_SHFT                                                               0x0

#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_ADDR                                                                (TURING_CC_REG_BASE      + 0x00001020)
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_RMSK                                                                0xffffffff
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_ADDR, HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_IN)
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_ENABLE_TRAINING_SEQ_BMSK                                            0x80000000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_ENABLE_TRAINING_SEQ_SHFT                                                  0x1f
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_BYPASS_LOGIC_DEP_BMSK                                               0x40000000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_BYPASS_LOGIC_DEP_SHFT                                                     0x1e
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_BYPASS_BIAS_DET_BMSK                                                0x20000000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_BYPASS_BIAS_DET_SHFT                                                      0x1d
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_MSB_CLOCK_SELECT_BMSK                                               0x10000000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_MSB_CLOCK_SELECT_SHFT                                                     0x1c
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_BMSK                                          0x8000000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_SHFT                                               0x1b
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_BMSK                                       0x6000000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_SHFT                                            0x19
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_MIRROR_DEVICE_ADJUST_BMSK                                            0x1c00000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_MIRROR_DEVICE_ADJUST_SHFT                                                 0x16
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_BMSK                                             0x200000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_SHFT                                                 0x15
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_DISABLE_CLAMP_BMSK                                                    0x100000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_DISABLE_CLAMP_SHFT                                                        0x14
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_BYPASS_MODE_OF_BIAS_BMSK                                               0x80000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_BYPASS_MODE_OF_BIAS_SHFT                                                  0x13
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_BMSK                                           0x40000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_SHFT                                              0x12
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_BIAS_ADJUST_BMSK                                                       0x30000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_BIAS_ADJUST_SHFT                                                          0x10
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_DIV2_NMO_EN_BMSK                                                        0x8000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_DIV2_NMO_EN_SHFT                                                           0xf
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_DIS_LEAK_CMP_BMSK                                                       0x4000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_DIS_LEAK_CMP_SHFT                                                          0xe
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_SINGLE_DMET_EN_BMSK                                                     0x2000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_SINGLE_DMET_EN_SHFT                                                        0xd
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_DEMET_WINDOW_DIS_BMSK                                                   0x1000
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_DEMET_WINDOW_DIS_SHFT                                                      0xc
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_NMO_OSC_SEL_BMSK                                                         0xc00
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_NMO_OSC_SEL_SHFT                                                           0xa
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_NOISE_MAG_BMSK                                                           0x380
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_NOISE_MAG_SHFT                                                             0x7
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_NOISE_GEN_EN_BMSK                                                         0x40
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_NOISE_GEN_EN_SHFT                                                          0x6
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_OSC_BIAS_GND_BMSK                                                         0x20
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_OSC_BIAS_GND_SHFT                                                          0x5
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_BMSK                                                     0x18
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_SHFT                                                      0x3
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_STATUS_REG_EN_BMSK                                                         0x4
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_STATUS_REG_EN_SHFT                                                         0x2
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_SKIP_FINE_TUNE_BMSK                                                        0x2
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_SKIP_FINE_TUNE_SHFT                                                        0x1
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_SKIP_FAST_LOCK_BMSK                                                        0x1
#define HWIO_TURING_TURING_DIG_PLL_TEST_CTL_U_SKIP_FAST_LOCK_SHFT                                                        0x0

#define HWIO_TURING_TURING_DIG_PLL_MODE_ADDR                                                                      (TURING_CC_REG_BASE      + 0x00001000)
#define HWIO_TURING_TURING_DIG_PLL_MODE_RMSK                                                                      0xffffffff
#define HWIO_TURING_TURING_DIG_PLL_MODE_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_MODE_ADDR, HWIO_TURING_TURING_DIG_PLL_MODE_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_MODE_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_MODE_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_MODE_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_MODE_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_MODE_IN)
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_LOCK_DET_BMSK                                                         0x80000000
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_LOCK_DET_SHFT                                                               0x1f
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                                                      0x40000000
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                                                            0x1e
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_ACK_LATCH_BMSK                                                        0x20000000
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_ACK_LATCH_SHFT                                                              0x1d
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_LOCK_DET_FINE_BMSK                                                    0x10000000
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_LOCK_DET_FINE_SHFT                                                          0x1c
#define HWIO_TURING_TURING_DIG_PLL_MODE_RESERVE_BITS27_25_BMSK                                                     0xe000000
#define HWIO_TURING_TURING_DIG_PLL_MODE_RESERVE_BITS27_25_SHFT                                                          0x19
#define HWIO_TURING_TURING_DIG_PLL_MODE_FSM_LEGACY_MODE_BMSK                                                       0x1000000
#define HWIO_TURING_TURING_DIG_PLL_MODE_FSM_LEGACY_MODE_SHFT                                                            0x18
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK                                             0x800000
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT                                                 0x17
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_UPDATE_BMSK                                                             0x400000
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_UPDATE_SHFT                                                                 0x16
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                                                     0x200000
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                                                         0x15
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                                                       0x100000
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                                                           0x14
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_BIAS_COUNT_BMSK                                                          0xfc000
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_BIAS_COUNT_SHFT                                                              0xe
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_LOCK_COUNT_BMSK                                                           0x3f00
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_LOCK_COUNT_SHFT                                                              0x8
#define HWIO_TURING_TURING_DIG_PLL_MODE_RESERVE_BITS7_3_BMSK                                                            0xf8
#define HWIO_TURING_TURING_DIG_PLL_MODE_RESERVE_BITS7_3_SHFT                                                             0x3
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_RESET_N_BMSK                                                                 0x4
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_RESET_N_SHFT                                                                 0x2
#define HWIO_TURING_TURING_DIG_PLL_MODE_RESERVE_BIT1_BMSK                                                                0x2
#define HWIO_TURING_TURING_DIG_PLL_MODE_RESERVE_BIT1_SHFT                                                                0x1
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_OUTCTRL_BMSK                                                                 0x1
#define HWIO_TURING_TURING_DIG_PLL_MODE_PLL_OUTCTRL_SHFT                                                                 0x0

#define HWIO_TURING_TURING_DIG_PLL_STATUS_ADDR                                                                    (TURING_CC_REG_BASE      + 0x00001024)
#define HWIO_TURING_TURING_DIG_PLL_STATUS_RMSK                                                                    0xffffffff
#define HWIO_TURING_TURING_DIG_PLL_STATUS_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_STATUS_ADDR, HWIO_TURING_TURING_DIG_PLL_STATUS_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_STATUS_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_STATUS_STATUS_31_0_BMSK                                                        0xffffffff
#define HWIO_TURING_TURING_DIG_PLL_STATUS_STATUS_31_0_SHFT                                                               0x0

#define HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_ADDR                                                                  (TURING_CC_REG_BASE      + 0x00001028)
#define HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_RMSK                                                                  0xffffffff
#define HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_ADDR, HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_IN)
#define HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_RESERVE_BITS31_27_BMSK                                                0xf8000000
#define HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_RESERVE_BITS31_27_SHFT                                                      0x1b
#define HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_FCW_PCAL_BMSK                                                          0x7000000
#define HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_FCW_PCAL_SHFT                                                               0x18
#define HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_FCW_ACC_BMSK                                                            0xffffff
#define HWIO_TURING_TURING_DIG_PLL_FREQ_CTL_FCW_ACC_SHFT                                                                 0x0

#define HWIO_TURING_TURING_DIG_PLL_OPMODE_ADDR                                                                    (TURING_CC_REG_BASE      + 0x0000102c)
#define HWIO_TURING_TURING_DIG_PLL_OPMODE_RMSK                                                                           0x7
#define HWIO_TURING_TURING_DIG_PLL_OPMODE_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_OPMODE_ADDR, HWIO_TURING_TURING_DIG_PLL_OPMODE_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_OPMODE_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_OPMODE_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_OPMODE_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_OPMODE_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_OPMODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_OPMODE_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_OPMODE_IN)
#define HWIO_TURING_TURING_DIG_PLL_OPMODE_PLL_OPMODE_BMSK                                                                0x7
#define HWIO_TURING_TURING_DIG_PLL_OPMODE_PLL_OPMODE_SHFT                                                                0x0

#define HWIO_TURING_TURING_DIG_PLL_STATE_ADDR                                                                     (TURING_CC_REG_BASE      + 0x00001030)
#define HWIO_TURING_TURING_DIG_PLL_STATE_RMSK                                                                            0x7
#define HWIO_TURING_TURING_DIG_PLL_STATE_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_STATE_ADDR, HWIO_TURING_TURING_DIG_PLL_STATE_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_STATE_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_STATE_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_STATE_PLL_STATE_BMSK                                                                  0x7
#define HWIO_TURING_TURING_DIG_PLL_STATE_PLL_STATE_SHFT                                                                  0x0

#define HWIO_TURING_TURING_DIG_PLL_DROOP_ADDR                                                                     (TURING_CC_REG_BASE      + 0x00001034)
#define HWIO_TURING_TURING_DIG_PLL_DROOP_RMSK                                                                           0xff
#define HWIO_TURING_TURING_DIG_PLL_DROOP_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_DROOP_ADDR, HWIO_TURING_TURING_DIG_PLL_DROOP_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_DROOP_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_DROOP_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_DROOP_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_DROOP_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_DROOP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_DROOP_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_DROOP_IN)
#define HWIO_TURING_TURING_DIG_PLL_DROOP_DROOP_CODE_BMSK                                                                0xfe
#define HWIO_TURING_TURING_DIG_PLL_DROOP_DROOP_CODE_SHFT                                                                 0x1
#define HWIO_TURING_TURING_DIG_PLL_DROOP_DROOP_EN_BMSK                                                                   0x1
#define HWIO_TURING_TURING_DIG_PLL_DROOP_DROOP_EN_SHFT                                                                   0x0

#define HWIO_TURING_TURING_DIG_PLL_FRAC_VAL_ADDR                                                                  (TURING_CC_REG_BASE      + 0x00001038)
#define HWIO_TURING_TURING_DIG_PLL_FRAC_VAL_RMSK                                                                      0xffff
#define HWIO_TURING_TURING_DIG_PLL_FRAC_VAL_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_FRAC_VAL_ADDR, HWIO_TURING_TURING_DIG_PLL_FRAC_VAL_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_FRAC_VAL_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_FRAC_VAL_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_FRAC_VAL_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_FRAC_VAL_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_FRAC_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_FRAC_VAL_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_FRAC_VAL_IN)
#define HWIO_TURING_TURING_DIG_PLL_FRAC_VAL_PLL_FRAC_VAL_BMSK                                                         0xffff
#define HWIO_TURING_TURING_DIG_PLL_FRAC_VAL_PLL_FRAC_VAL_SHFT                                                            0x0

#define HWIO_TURING_TURING_DIG_PLL_SPARE_ADDR                                                                     (TURING_CC_REG_BASE      + 0x0000103c)
#define HWIO_TURING_TURING_DIG_PLL_SPARE_RMSK                                                                           0xff
#define HWIO_TURING_TURING_DIG_PLL_SPARE_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_SPARE_ADDR, HWIO_TURING_TURING_DIG_PLL_SPARE_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_SPARE_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_SPARE_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_SPARE_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_SPARE_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_SPARE_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_SPARE_IN)
#define HWIO_TURING_TURING_DIG_PLL_SPARE_SPARE_OUTPUTS_BMSK                                                             0xf0
#define HWIO_TURING_TURING_DIG_PLL_SPARE_SPARE_OUTPUTS_SHFT                                                              0x4
#define HWIO_TURING_TURING_DIG_PLL_SPARE_SPARE_INPUTS_BMSK                                                               0xf
#define HWIO_TURING_TURING_DIG_PLL_SPARE_SPARE_INPUTS_SHFT                                                               0x0

#define HWIO_TURING_TURING_DIG_PLL_SSC_DELTA_ALPHA_ADDR                                                           (TURING_CC_REG_BASE      + 0x00001040)
#define HWIO_TURING_TURING_DIG_PLL_SSC_DELTA_ALPHA_RMSK                                                                 0xff
#define HWIO_TURING_TURING_DIG_PLL_SSC_DELTA_ALPHA_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_SSC_DELTA_ALPHA_ADDR, HWIO_TURING_TURING_DIG_PLL_SSC_DELTA_ALPHA_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_SSC_DELTA_ALPHA_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_SSC_DELTA_ALPHA_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_SSC_DELTA_ALPHA_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_SSC_DELTA_ALPHA_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_SSC_DELTA_ALPHA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_SSC_DELTA_ALPHA_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_SSC_DELTA_ALPHA_IN)
#define HWIO_TURING_TURING_DIG_PLL_SSC_DELTA_ALPHA_SSC_DELTA_ALPHA_BMSK                                                 0xff
#define HWIO_TURING_TURING_DIG_PLL_SSC_DELTA_ALPHA_SSC_DELTA_ALPHA_SHFT                                                  0x0

#define HWIO_TURING_TURING_DIG_PLL_SSC_NUM_STEPS_ADDR                                                             (TURING_CC_REG_BASE      + 0x00001044)
#define HWIO_TURING_TURING_DIG_PLL_SSC_NUM_STEPS_RMSK                                                                    0xf
#define HWIO_TURING_TURING_DIG_PLL_SSC_NUM_STEPS_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_SSC_NUM_STEPS_ADDR, HWIO_TURING_TURING_DIG_PLL_SSC_NUM_STEPS_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_SSC_NUM_STEPS_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_SSC_NUM_STEPS_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_SSC_NUM_STEPS_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_SSC_NUM_STEPS_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_SSC_NUM_STEPS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_SSC_NUM_STEPS_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_SSC_NUM_STEPS_IN)
#define HWIO_TURING_TURING_DIG_PLL_SSC_NUM_STEPS_SSC_NUM_STEPS_BMSK                                                      0xf
#define HWIO_TURING_TURING_DIG_PLL_SSC_NUM_STEPS_SSC_NUM_STEPS_SHFT                                                      0x0

#define HWIO_TURING_TURING_DIG_PLL_SSC_UPDATE_RATE_ADDR                                                           (TURING_CC_REG_BASE      + 0x00001048)
#define HWIO_TURING_TURING_DIG_PLL_SSC_UPDATE_RATE_RMSK                                                                  0xf
#define HWIO_TURING_TURING_DIG_PLL_SSC_UPDATE_RATE_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_SSC_UPDATE_RATE_ADDR, HWIO_TURING_TURING_DIG_PLL_SSC_UPDATE_RATE_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_SSC_UPDATE_RATE_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_SSC_UPDATE_RATE_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_SSC_UPDATE_RATE_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_SSC_UPDATE_RATE_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_SSC_UPDATE_RATE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_SSC_UPDATE_RATE_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_SSC_UPDATE_RATE_IN)
#define HWIO_TURING_TURING_DIG_PLL_SSC_UPDATE_RATE_SSC_UPDATE_RATE_BMSK                                                  0xf
#define HWIO_TURING_TURING_DIG_PLL_SSC_UPDATE_RATE_SSC_UPDATE_RATE_SHFT                                                  0x0

#define HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_ADDR                                                                (TURING_CC_REG_BASE      + 0x0000104c)
#define HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_RMSK                                                                       0xf
#define HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_ADDR, HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_IN)
#define HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_CLK_ODD_CGC_EN_BMSK                                                        0x8
#define HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_CLK_ODD_CGC_EN_SHFT                                                        0x3
#define HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_CLK_EVEN_CGC_EN_BMSK                                                       0x4
#define HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_CLK_EVEN_CGC_EN_SHFT                                                       0x2
#define HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_CLK_MAIN_SVS_CGC_EN_BMSK                                                   0x2
#define HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_CLK_MAIN_SVS_CGC_EN_SHFT                                                   0x1
#define HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_CLK_MAIN_CGC_EN_BMSK                                                       0x1
#define HWIO_TURING_TURING_DIG_PLL_CLK_CGC_EN_CLK_MAIN_CGC_EN_SHFT                                                       0x0

#define HWIO_TURING_TURING_DIG_PLL_CLK_DIV_ADDR                                                                   (TURING_CC_REG_BASE      + 0x00001050)
#define HWIO_TURING_TURING_DIG_PLL_CLK_DIV_RMSK                                                                          0x3
#define HWIO_TURING_TURING_DIG_PLL_CLK_DIV_IN          \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_CLK_DIV_ADDR, HWIO_TURING_TURING_DIG_PLL_CLK_DIV_RMSK)
#define HWIO_TURING_TURING_DIG_PLL_CLK_DIV_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_DIG_PLL_CLK_DIV_ADDR, m)
#define HWIO_TURING_TURING_DIG_PLL_CLK_DIV_OUT(v)      \
        out_dword(HWIO_TURING_TURING_DIG_PLL_CLK_DIV_ADDR,v)
#define HWIO_TURING_TURING_DIG_PLL_CLK_DIV_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_DIG_PLL_CLK_DIV_ADDR,m,v,HWIO_TURING_TURING_DIG_PLL_CLK_DIV_IN)
#define HWIO_TURING_TURING_DIG_PLL_CLK_DIV_CLK_MAIN_DIV_BMSK                                                             0x3
#define HWIO_TURING_TURING_DIG_PLL_CLK_DIV_CLK_MAIN_DIV_SHFT                                                             0x0

#define HWIO_TURING_Q6SS_BCR_ADDR                                                                                 (TURING_CC_REG_BASE      + 0x00003000)
#define HWIO_TURING_Q6SS_BCR_RMSK                                                                                 0x80000003
#define HWIO_TURING_Q6SS_BCR_IN          \
        in_dword_masked(HWIO_TURING_Q6SS_BCR_ADDR, HWIO_TURING_Q6SS_BCR_RMSK)
#define HWIO_TURING_Q6SS_BCR_INM(m)      \
        in_dword_masked(HWIO_TURING_Q6SS_BCR_ADDR, m)
#define HWIO_TURING_Q6SS_BCR_OUT(v)      \
        out_dword(HWIO_TURING_Q6SS_BCR_ADDR,v)
#define HWIO_TURING_Q6SS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_Q6SS_BCR_ADDR,m,v,HWIO_TURING_Q6SS_BCR_IN)
#define HWIO_TURING_Q6SS_BCR_DFD_STATUS_BMSK                                                                      0x80000000
#define HWIO_TURING_Q6SS_BCR_DFD_STATUS_SHFT                                                                            0x1f
#define HWIO_TURING_Q6SS_BCR_DFD_EN_BMSK                                                                                 0x2
#define HWIO_TURING_Q6SS_BCR_DFD_EN_SHFT                                                                                 0x1
#define HWIO_TURING_Q6SS_BCR_BLK_ARES_BMSK                                                                               0x1
#define HWIO_TURING_Q6SS_BCR_BLK_ARES_SHFT                                                                               0x0

#define HWIO_TURING_Q6SS_BCR_SLP_CBCR_ADDR                                                                        (TURING_CC_REG_BASE      + 0x00003004)
#define HWIO_TURING_Q6SS_BCR_SLP_CBCR_RMSK                                                                        0x80000003
#define HWIO_TURING_Q6SS_BCR_SLP_CBCR_IN          \
        in_dword_masked(HWIO_TURING_Q6SS_BCR_SLP_CBCR_ADDR, HWIO_TURING_Q6SS_BCR_SLP_CBCR_RMSK)
#define HWIO_TURING_Q6SS_BCR_SLP_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_Q6SS_BCR_SLP_CBCR_ADDR, m)
#define HWIO_TURING_Q6SS_BCR_SLP_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_Q6SS_BCR_SLP_CBCR_ADDR,v)
#define HWIO_TURING_Q6SS_BCR_SLP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_Q6SS_BCR_SLP_CBCR_ADDR,m,v,HWIO_TURING_Q6SS_BCR_SLP_CBCR_IN)
#define HWIO_TURING_Q6SS_BCR_SLP_CBCR_CLK_OFF_BMSK                                                                0x80000000
#define HWIO_TURING_Q6SS_BCR_SLP_CBCR_CLK_OFF_SHFT                                                                      0x1f
#define HWIO_TURING_Q6SS_BCR_SLP_CBCR_HW_CTL_BMSK                                                                        0x2
#define HWIO_TURING_Q6SS_BCR_SLP_CBCR_HW_CTL_SHFT                                                                        0x1
#define HWIO_TURING_Q6SS_BCR_SLP_CBCR_CLK_ENABLE_BMSK                                                                    0x1
#define HWIO_TURING_Q6SS_BCR_SLP_CBCR_CLK_ENABLE_SHFT                                                                    0x0

#define HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_ADDR                                                                  (TURING_CC_REG_BASE      + 0x00004000)
#define HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_RMSK                                                                  0x80000003
#define HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_IN          \
        in_dword_masked(HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_ADDR, HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_RMSK)
#define HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_ADDR, m)
#define HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_ADDR,v)
#define HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_ADDR,m,v,HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_IN)
#define HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_CLK_OFF_BMSK                                                          0x80000000
#define HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_CLK_OFF_SHFT                                                                0x1f
#define HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_HW_CTL_BMSK                                                                  0x2
#define HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_HW_CTL_SHFT                                                                  0x1
#define HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_CLK_ENABLE_BMSK                                                              0x1
#define HWIO_TURING_Q6SS_ALT_RESET_AON_CBCR_CLK_ENABLE_SHFT                                                              0x0

#define HWIO_TURING_Q6SS_ALT_RESET_CTL_ADDR                                                                       (TURING_CC_REG_BASE      + 0x00004004)
#define HWIO_TURING_Q6SS_ALT_RESET_CTL_RMSK                                                                              0x1
#define HWIO_TURING_Q6SS_ALT_RESET_CTL_IN          \
        in_dword_masked(HWIO_TURING_Q6SS_ALT_RESET_CTL_ADDR, HWIO_TURING_Q6SS_ALT_RESET_CTL_RMSK)
#define HWIO_TURING_Q6SS_ALT_RESET_CTL_INM(m)      \
        in_dword_masked(HWIO_TURING_Q6SS_ALT_RESET_CTL_ADDR, m)
#define HWIO_TURING_Q6SS_ALT_RESET_CTL_OUT(v)      \
        out_dword(HWIO_TURING_Q6SS_ALT_RESET_CTL_ADDR,v)
#define HWIO_TURING_Q6SS_ALT_RESET_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_Q6SS_ALT_RESET_CTL_ADDR,m,v,HWIO_TURING_Q6SS_ALT_RESET_CTL_IN)
#define HWIO_TURING_Q6SS_ALT_RESET_CTL_ALT_ARES_BYPASS_BMSK                                                              0x1
#define HWIO_TURING_Q6SS_ALT_RESET_CTL_ALT_ARES_BYPASS_SHFT                                                              0x0

#define HWIO_TURING_AON_CMD_RCGR_ADDR                                                                             (TURING_CC_REG_BASE      + 0x00005000)
#define HWIO_TURING_AON_CMD_RCGR_RMSK                                                                             0x800000f3
#define HWIO_TURING_AON_CMD_RCGR_IN          \
        in_dword_masked(HWIO_TURING_AON_CMD_RCGR_ADDR, HWIO_TURING_AON_CMD_RCGR_RMSK)
#define HWIO_TURING_AON_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_CMD_RCGR_ADDR, m)
#define HWIO_TURING_AON_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_TURING_AON_CMD_RCGR_ADDR,v)
#define HWIO_TURING_AON_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_CMD_RCGR_ADDR,m,v,HWIO_TURING_AON_CMD_RCGR_IN)
#define HWIO_TURING_AON_CMD_RCGR_ROOT_OFF_BMSK                                                                    0x80000000
#define HWIO_TURING_AON_CMD_RCGR_ROOT_OFF_SHFT                                                                          0x1f
#define HWIO_TURING_AON_CMD_RCGR_DIRTY_D_BMSK                                                                           0x80
#define HWIO_TURING_AON_CMD_RCGR_DIRTY_D_SHFT                                                                            0x7
#define HWIO_TURING_AON_CMD_RCGR_DIRTY_N_BMSK                                                                           0x40
#define HWIO_TURING_AON_CMD_RCGR_DIRTY_N_SHFT                                                                            0x6
#define HWIO_TURING_AON_CMD_RCGR_DIRTY_M_BMSK                                                                           0x20
#define HWIO_TURING_AON_CMD_RCGR_DIRTY_M_SHFT                                                                            0x5
#define HWIO_TURING_AON_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                                    0x10
#define HWIO_TURING_AON_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                                     0x4
#define HWIO_TURING_AON_CMD_RCGR_ROOT_EN_BMSK                                                                            0x2
#define HWIO_TURING_AON_CMD_RCGR_ROOT_EN_SHFT                                                                            0x1
#define HWIO_TURING_AON_CMD_RCGR_UPDATE_BMSK                                                                             0x1
#define HWIO_TURING_AON_CMD_RCGR_UPDATE_SHFT                                                                             0x0

#define HWIO_TURING_AON_CFG_RCGR_ADDR                                                                             (TURING_CC_REG_BASE      + 0x00005004)
#define HWIO_TURING_AON_CFG_RCGR_RMSK                                                                                 0x771f
#define HWIO_TURING_AON_CFG_RCGR_IN          \
        in_dword_masked(HWIO_TURING_AON_CFG_RCGR_ADDR, HWIO_TURING_AON_CFG_RCGR_RMSK)
#define HWIO_TURING_AON_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_CFG_RCGR_ADDR, m)
#define HWIO_TURING_AON_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_TURING_AON_CFG_RCGR_ADDR,v)
#define HWIO_TURING_AON_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_CFG_RCGR_ADDR,m,v,HWIO_TURING_AON_CFG_RCGR_IN)
#define HWIO_TURING_AON_CFG_RCGR_ALT_SRC_SEL_BMSK                                                                     0x4000
#define HWIO_TURING_AON_CFG_RCGR_ALT_SRC_SEL_SHFT                                                                        0xe
#define HWIO_TURING_AON_CFG_RCGR_MODE_BMSK                                                                            0x3000
#define HWIO_TURING_AON_CFG_RCGR_MODE_SHFT                                                                               0xc
#define HWIO_TURING_AON_CFG_RCGR_SRC_SEL_BMSK                                                                          0x700
#define HWIO_TURING_AON_CFG_RCGR_SRC_SEL_SHFT                                                                            0x8
#define HWIO_TURING_AON_CFG_RCGR_SRC_DIV_BMSK                                                                           0x1f
#define HWIO_TURING_AON_CFG_RCGR_SRC_DIV_SHFT                                                                            0x0

#define HWIO_TURING_AON_M_ADDR                                                                                    (TURING_CC_REG_BASE      + 0x00005008)
#define HWIO_TURING_AON_M_RMSK                                                                                          0xff
#define HWIO_TURING_AON_M_IN          \
        in_dword_masked(HWIO_TURING_AON_M_ADDR, HWIO_TURING_AON_M_RMSK)
#define HWIO_TURING_AON_M_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_M_ADDR, m)
#define HWIO_TURING_AON_M_OUT(v)      \
        out_dword(HWIO_TURING_AON_M_ADDR,v)
#define HWIO_TURING_AON_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_M_ADDR,m,v,HWIO_TURING_AON_M_IN)
#define HWIO_TURING_AON_M_M_BMSK                                                                                        0xff
#define HWIO_TURING_AON_M_M_SHFT                                                                                         0x0

#define HWIO_TURING_AON_N_ADDR                                                                                    (TURING_CC_REG_BASE      + 0x0000500c)
#define HWIO_TURING_AON_N_RMSK                                                                                          0xff
#define HWIO_TURING_AON_N_IN          \
        in_dword_masked(HWIO_TURING_AON_N_ADDR, HWIO_TURING_AON_N_RMSK)
#define HWIO_TURING_AON_N_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_N_ADDR, m)
#define HWIO_TURING_AON_N_OUT(v)      \
        out_dword(HWIO_TURING_AON_N_ADDR,v)
#define HWIO_TURING_AON_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_N_ADDR,m,v,HWIO_TURING_AON_N_IN)
#define HWIO_TURING_AON_N_NOT_N_MINUS_M_BMSK                                                                            0xff
#define HWIO_TURING_AON_N_NOT_N_MINUS_M_SHFT                                                                             0x0

#define HWIO_TURING_AON_D_ADDR                                                                                    (TURING_CC_REG_BASE      + 0x00005010)
#define HWIO_TURING_AON_D_RMSK                                                                                          0xff
#define HWIO_TURING_AON_D_IN          \
        in_dword_masked(HWIO_TURING_AON_D_ADDR, HWIO_TURING_AON_D_RMSK)
#define HWIO_TURING_AON_D_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_D_ADDR, m)
#define HWIO_TURING_AON_D_OUT(v)      \
        out_dword(HWIO_TURING_AON_D_ADDR,v)
#define HWIO_TURING_AON_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_D_ADDR,m,v,HWIO_TURING_AON_D_IN)
#define HWIO_TURING_AON_D_NOT_2D_BMSK                                                                                   0xff
#define HWIO_TURING_AON_D_NOT_2D_SHFT                                                                                    0x0

#define HWIO_TURING_AON_CMD_DFSR_ADDR                                                                             (TURING_CC_REG_BASE      + 0x00005014)
#define HWIO_TURING_AON_CMD_DFSR_RMSK                                                                                 0xbfef
#define HWIO_TURING_AON_CMD_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_CMD_DFSR_ADDR, HWIO_TURING_AON_CMD_DFSR_RMSK)
#define HWIO_TURING_AON_CMD_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_CMD_DFSR_ADDR, m)
#define HWIO_TURING_AON_CMD_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_CMD_DFSR_ADDR,v)
#define HWIO_TURING_AON_CMD_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_CMD_DFSR_ADDR,m,v,HWIO_TURING_AON_CMD_DFSR_IN)
#define HWIO_TURING_AON_CMD_DFSR_RCG_SW_CTRL_BMSK                                                                     0x8000
#define HWIO_TURING_AON_CMD_DFSR_RCG_SW_CTRL_SHFT                                                                        0xf
#define HWIO_TURING_AON_CMD_DFSR_SW_PERF_STATE_BMSK                                                                   0x3800
#define HWIO_TURING_AON_CMD_DFSR_SW_PERF_STATE_SHFT                                                                      0xb
#define HWIO_TURING_AON_CMD_DFSR_SW_OVERRIDE_BMSK                                                                      0x400
#define HWIO_TURING_AON_CMD_DFSR_SW_OVERRIDE_SHFT                                                                        0xa
#define HWIO_TURING_AON_CMD_DFSR_PERF_STATE_UPDATE_STATUS_BMSK                                                         0x200
#define HWIO_TURING_AON_CMD_DFSR_PERF_STATE_UPDATE_STATUS_SHFT                                                           0x9
#define HWIO_TURING_AON_CMD_DFSR_DFS_FSM_STATE_BMSK                                                                    0x1c0
#define HWIO_TURING_AON_CMD_DFSR_DFS_FSM_STATE_SHFT                                                                      0x6
#define HWIO_TURING_AON_CMD_DFSR_HW_CLK_CONTROL_BMSK                                                                    0x20
#define HWIO_TURING_AON_CMD_DFSR_HW_CLK_CONTROL_SHFT                                                                     0x5
#define HWIO_TURING_AON_CMD_DFSR_CURR_PERF_STATE_BMSK                                                                    0xe
#define HWIO_TURING_AON_CMD_DFSR_CURR_PERF_STATE_SHFT                                                                    0x1
#define HWIO_TURING_AON_CMD_DFSR_DFS_EN_BMSK                                                                             0x1
#define HWIO_TURING_AON_CMD_DFSR_DFS_EN_SHFT                                                                             0x0

#define HWIO_TURING_AON_PERF0_DFSR_ADDR                                                                           (TURING_CC_REG_BASE      + 0x00005018)
#define HWIO_TURING_AON_PERF0_DFSR_RMSK                                                                               0x371f
#define HWIO_TURING_AON_PERF0_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF0_DFSR_ADDR, HWIO_TURING_AON_PERF0_DFSR_RMSK)
#define HWIO_TURING_AON_PERF0_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF0_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF0_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF0_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF0_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF0_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF0_DFSR_IN)
#define HWIO_TURING_AON_PERF0_DFSR_MODE_BMSK                                                                          0x3000
#define HWIO_TURING_AON_PERF0_DFSR_MODE_SHFT                                                                             0xc
#define HWIO_TURING_AON_PERF0_DFSR_SRC_SEL_BMSK                                                                        0x700
#define HWIO_TURING_AON_PERF0_DFSR_SRC_SEL_SHFT                                                                          0x8
#define HWIO_TURING_AON_PERF0_DFSR_SRC_DIV_BMSK                                                                         0x1f
#define HWIO_TURING_AON_PERF0_DFSR_SRC_DIV_SHFT                                                                          0x0

#define HWIO_TURING_AON_PERF1_DFSR_ADDR                                                                           (TURING_CC_REG_BASE      + 0x0000501c)
#define HWIO_TURING_AON_PERF1_DFSR_RMSK                                                                               0x371f
#define HWIO_TURING_AON_PERF1_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF1_DFSR_ADDR, HWIO_TURING_AON_PERF1_DFSR_RMSK)
#define HWIO_TURING_AON_PERF1_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF1_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF1_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF1_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF1_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF1_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF1_DFSR_IN)
#define HWIO_TURING_AON_PERF1_DFSR_MODE_BMSK                                                                          0x3000
#define HWIO_TURING_AON_PERF1_DFSR_MODE_SHFT                                                                             0xc
#define HWIO_TURING_AON_PERF1_DFSR_SRC_SEL_BMSK                                                                        0x700
#define HWIO_TURING_AON_PERF1_DFSR_SRC_SEL_SHFT                                                                          0x8
#define HWIO_TURING_AON_PERF1_DFSR_SRC_DIV_BMSK                                                                         0x1f
#define HWIO_TURING_AON_PERF1_DFSR_SRC_DIV_SHFT                                                                          0x0

#define HWIO_TURING_AON_PERF2_DFSR_ADDR                                                                           (TURING_CC_REG_BASE      + 0x00005020)
#define HWIO_TURING_AON_PERF2_DFSR_RMSK                                                                               0x371f
#define HWIO_TURING_AON_PERF2_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF2_DFSR_ADDR, HWIO_TURING_AON_PERF2_DFSR_RMSK)
#define HWIO_TURING_AON_PERF2_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF2_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF2_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF2_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF2_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF2_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF2_DFSR_IN)
#define HWIO_TURING_AON_PERF2_DFSR_MODE_BMSK                                                                          0x3000
#define HWIO_TURING_AON_PERF2_DFSR_MODE_SHFT                                                                             0xc
#define HWIO_TURING_AON_PERF2_DFSR_SRC_SEL_BMSK                                                                        0x700
#define HWIO_TURING_AON_PERF2_DFSR_SRC_SEL_SHFT                                                                          0x8
#define HWIO_TURING_AON_PERF2_DFSR_SRC_DIV_BMSK                                                                         0x1f
#define HWIO_TURING_AON_PERF2_DFSR_SRC_DIV_SHFT                                                                          0x0

#define HWIO_TURING_AON_PERF3_DFSR_ADDR                                                                           (TURING_CC_REG_BASE      + 0x00005024)
#define HWIO_TURING_AON_PERF3_DFSR_RMSK                                                                               0x371f
#define HWIO_TURING_AON_PERF3_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF3_DFSR_ADDR, HWIO_TURING_AON_PERF3_DFSR_RMSK)
#define HWIO_TURING_AON_PERF3_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF3_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF3_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF3_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF3_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF3_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF3_DFSR_IN)
#define HWIO_TURING_AON_PERF3_DFSR_MODE_BMSK                                                                          0x3000
#define HWIO_TURING_AON_PERF3_DFSR_MODE_SHFT                                                                             0xc
#define HWIO_TURING_AON_PERF3_DFSR_SRC_SEL_BMSK                                                                        0x700
#define HWIO_TURING_AON_PERF3_DFSR_SRC_SEL_SHFT                                                                          0x8
#define HWIO_TURING_AON_PERF3_DFSR_SRC_DIV_BMSK                                                                         0x1f
#define HWIO_TURING_AON_PERF3_DFSR_SRC_DIV_SHFT                                                                          0x0

#define HWIO_TURING_AON_PERF4_DFSR_ADDR                                                                           (TURING_CC_REG_BASE      + 0x00005028)
#define HWIO_TURING_AON_PERF4_DFSR_RMSK                                                                               0x371f
#define HWIO_TURING_AON_PERF4_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF4_DFSR_ADDR, HWIO_TURING_AON_PERF4_DFSR_RMSK)
#define HWIO_TURING_AON_PERF4_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF4_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF4_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF4_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF4_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF4_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF4_DFSR_IN)
#define HWIO_TURING_AON_PERF4_DFSR_MODE_BMSK                                                                          0x3000
#define HWIO_TURING_AON_PERF4_DFSR_MODE_SHFT                                                                             0xc
#define HWIO_TURING_AON_PERF4_DFSR_SRC_SEL_BMSK                                                                        0x700
#define HWIO_TURING_AON_PERF4_DFSR_SRC_SEL_SHFT                                                                          0x8
#define HWIO_TURING_AON_PERF4_DFSR_SRC_DIV_BMSK                                                                         0x1f
#define HWIO_TURING_AON_PERF4_DFSR_SRC_DIV_SHFT                                                                          0x0

#define HWIO_TURING_AON_PERF5_DFSR_ADDR                                                                           (TURING_CC_REG_BASE      + 0x0000502c)
#define HWIO_TURING_AON_PERF5_DFSR_RMSK                                                                               0x371f
#define HWIO_TURING_AON_PERF5_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF5_DFSR_ADDR, HWIO_TURING_AON_PERF5_DFSR_RMSK)
#define HWIO_TURING_AON_PERF5_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF5_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF5_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF5_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF5_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF5_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF5_DFSR_IN)
#define HWIO_TURING_AON_PERF5_DFSR_MODE_BMSK                                                                          0x3000
#define HWIO_TURING_AON_PERF5_DFSR_MODE_SHFT                                                                             0xc
#define HWIO_TURING_AON_PERF5_DFSR_SRC_SEL_BMSK                                                                        0x700
#define HWIO_TURING_AON_PERF5_DFSR_SRC_SEL_SHFT                                                                          0x8
#define HWIO_TURING_AON_PERF5_DFSR_SRC_DIV_BMSK                                                                         0x1f
#define HWIO_TURING_AON_PERF5_DFSR_SRC_DIV_SHFT                                                                          0x0

#define HWIO_TURING_AON_PERF6_DFSR_ADDR                                                                           (TURING_CC_REG_BASE      + 0x00005030)
#define HWIO_TURING_AON_PERF6_DFSR_RMSK                                                                               0x371f
#define HWIO_TURING_AON_PERF6_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF6_DFSR_ADDR, HWIO_TURING_AON_PERF6_DFSR_RMSK)
#define HWIO_TURING_AON_PERF6_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF6_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF6_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF6_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF6_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF6_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF6_DFSR_IN)
#define HWIO_TURING_AON_PERF6_DFSR_MODE_BMSK                                                                          0x3000
#define HWIO_TURING_AON_PERF6_DFSR_MODE_SHFT                                                                             0xc
#define HWIO_TURING_AON_PERF6_DFSR_SRC_SEL_BMSK                                                                        0x700
#define HWIO_TURING_AON_PERF6_DFSR_SRC_SEL_SHFT                                                                          0x8
#define HWIO_TURING_AON_PERF6_DFSR_SRC_DIV_BMSK                                                                         0x1f
#define HWIO_TURING_AON_PERF6_DFSR_SRC_DIV_SHFT                                                                          0x0

#define HWIO_TURING_AON_PERF7_DFSR_ADDR                                                                           (TURING_CC_REG_BASE      + 0x00005034)
#define HWIO_TURING_AON_PERF7_DFSR_RMSK                                                                               0x371f
#define HWIO_TURING_AON_PERF7_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF7_DFSR_ADDR, HWIO_TURING_AON_PERF7_DFSR_RMSK)
#define HWIO_TURING_AON_PERF7_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF7_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF7_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF7_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF7_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF7_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF7_DFSR_IN)
#define HWIO_TURING_AON_PERF7_DFSR_MODE_BMSK                                                                          0x3000
#define HWIO_TURING_AON_PERF7_DFSR_MODE_SHFT                                                                             0xc
#define HWIO_TURING_AON_PERF7_DFSR_SRC_SEL_BMSK                                                                        0x700
#define HWIO_TURING_AON_PERF7_DFSR_SRC_SEL_SHFT                                                                          0x8
#define HWIO_TURING_AON_PERF7_DFSR_SRC_DIV_BMSK                                                                         0x1f
#define HWIO_TURING_AON_PERF7_DFSR_SRC_DIV_SHFT                                                                          0x0

#define HWIO_TURING_AON_PERF0_M_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005038)
#define HWIO_TURING_AON_PERF0_M_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF0_M_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF0_M_DFSR_ADDR, HWIO_TURING_AON_PERF0_M_DFSR_RMSK)
#define HWIO_TURING_AON_PERF0_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF0_M_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF0_M_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF0_M_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF0_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF0_M_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF0_M_DFSR_IN)
#define HWIO_TURING_AON_PERF0_M_DFSR_M_BMSK                                                                             0xff
#define HWIO_TURING_AON_PERF0_M_DFSR_M_SHFT                                                                              0x0

#define HWIO_TURING_AON_PERF1_M_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x0000503c)
#define HWIO_TURING_AON_PERF1_M_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF1_M_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF1_M_DFSR_ADDR, HWIO_TURING_AON_PERF1_M_DFSR_RMSK)
#define HWIO_TURING_AON_PERF1_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF1_M_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF1_M_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF1_M_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF1_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF1_M_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF1_M_DFSR_IN)
#define HWIO_TURING_AON_PERF1_M_DFSR_M_BMSK                                                                             0xff
#define HWIO_TURING_AON_PERF1_M_DFSR_M_SHFT                                                                              0x0

#define HWIO_TURING_AON_PERF2_M_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005040)
#define HWIO_TURING_AON_PERF2_M_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF2_M_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF2_M_DFSR_ADDR, HWIO_TURING_AON_PERF2_M_DFSR_RMSK)
#define HWIO_TURING_AON_PERF2_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF2_M_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF2_M_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF2_M_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF2_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF2_M_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF2_M_DFSR_IN)
#define HWIO_TURING_AON_PERF2_M_DFSR_M_BMSK                                                                             0xff
#define HWIO_TURING_AON_PERF2_M_DFSR_M_SHFT                                                                              0x0

#define HWIO_TURING_AON_PERF3_M_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005044)
#define HWIO_TURING_AON_PERF3_M_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF3_M_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF3_M_DFSR_ADDR, HWIO_TURING_AON_PERF3_M_DFSR_RMSK)
#define HWIO_TURING_AON_PERF3_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF3_M_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF3_M_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF3_M_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF3_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF3_M_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF3_M_DFSR_IN)
#define HWIO_TURING_AON_PERF3_M_DFSR_M_BMSK                                                                             0xff
#define HWIO_TURING_AON_PERF3_M_DFSR_M_SHFT                                                                              0x0

#define HWIO_TURING_AON_PERF4_M_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005048)
#define HWIO_TURING_AON_PERF4_M_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF4_M_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF4_M_DFSR_ADDR, HWIO_TURING_AON_PERF4_M_DFSR_RMSK)
#define HWIO_TURING_AON_PERF4_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF4_M_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF4_M_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF4_M_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF4_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF4_M_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF4_M_DFSR_IN)
#define HWIO_TURING_AON_PERF4_M_DFSR_M_BMSK                                                                             0xff
#define HWIO_TURING_AON_PERF4_M_DFSR_M_SHFT                                                                              0x0

#define HWIO_TURING_AON_PERF5_M_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x0000504c)
#define HWIO_TURING_AON_PERF5_M_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF5_M_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF5_M_DFSR_ADDR, HWIO_TURING_AON_PERF5_M_DFSR_RMSK)
#define HWIO_TURING_AON_PERF5_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF5_M_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF5_M_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF5_M_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF5_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF5_M_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF5_M_DFSR_IN)
#define HWIO_TURING_AON_PERF5_M_DFSR_M_BMSK                                                                             0xff
#define HWIO_TURING_AON_PERF5_M_DFSR_M_SHFT                                                                              0x0

#define HWIO_TURING_AON_PERF6_M_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005050)
#define HWIO_TURING_AON_PERF6_M_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF6_M_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF6_M_DFSR_ADDR, HWIO_TURING_AON_PERF6_M_DFSR_RMSK)
#define HWIO_TURING_AON_PERF6_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF6_M_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF6_M_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF6_M_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF6_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF6_M_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF6_M_DFSR_IN)
#define HWIO_TURING_AON_PERF6_M_DFSR_M_BMSK                                                                             0xff
#define HWIO_TURING_AON_PERF6_M_DFSR_M_SHFT                                                                              0x0

#define HWIO_TURING_AON_PERF7_M_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005054)
#define HWIO_TURING_AON_PERF7_M_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF7_M_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF7_M_DFSR_ADDR, HWIO_TURING_AON_PERF7_M_DFSR_RMSK)
#define HWIO_TURING_AON_PERF7_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF7_M_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF7_M_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF7_M_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF7_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF7_M_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF7_M_DFSR_IN)
#define HWIO_TURING_AON_PERF7_M_DFSR_M_BMSK                                                                             0xff
#define HWIO_TURING_AON_PERF7_M_DFSR_M_SHFT                                                                              0x0

#define HWIO_TURING_AON_PERF0_N_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005058)
#define HWIO_TURING_AON_PERF0_N_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF0_N_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF0_N_DFSR_ADDR, HWIO_TURING_AON_PERF0_N_DFSR_RMSK)
#define HWIO_TURING_AON_PERF0_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF0_N_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF0_N_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF0_N_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF0_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF0_N_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF0_N_DFSR_IN)
#define HWIO_TURING_AON_PERF0_N_DFSR_NOT_N_MINUS_M_BMSK                                                                 0xff
#define HWIO_TURING_AON_PERF0_N_DFSR_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_TURING_AON_PERF1_N_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x0000505c)
#define HWIO_TURING_AON_PERF1_N_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF1_N_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF1_N_DFSR_ADDR, HWIO_TURING_AON_PERF1_N_DFSR_RMSK)
#define HWIO_TURING_AON_PERF1_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF1_N_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF1_N_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF1_N_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF1_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF1_N_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF1_N_DFSR_IN)
#define HWIO_TURING_AON_PERF1_N_DFSR_NOT_N_MINUS_M_BMSK                                                                 0xff
#define HWIO_TURING_AON_PERF1_N_DFSR_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_TURING_AON_PERF2_N_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005060)
#define HWIO_TURING_AON_PERF2_N_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF2_N_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF2_N_DFSR_ADDR, HWIO_TURING_AON_PERF2_N_DFSR_RMSK)
#define HWIO_TURING_AON_PERF2_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF2_N_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF2_N_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF2_N_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF2_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF2_N_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF2_N_DFSR_IN)
#define HWIO_TURING_AON_PERF2_N_DFSR_NOT_N_MINUS_M_BMSK                                                                 0xff
#define HWIO_TURING_AON_PERF2_N_DFSR_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_TURING_AON_PERF3_N_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005064)
#define HWIO_TURING_AON_PERF3_N_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF3_N_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF3_N_DFSR_ADDR, HWIO_TURING_AON_PERF3_N_DFSR_RMSK)
#define HWIO_TURING_AON_PERF3_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF3_N_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF3_N_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF3_N_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF3_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF3_N_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF3_N_DFSR_IN)
#define HWIO_TURING_AON_PERF3_N_DFSR_NOT_N_MINUS_M_BMSK                                                                 0xff
#define HWIO_TURING_AON_PERF3_N_DFSR_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_TURING_AON_PERF4_N_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005068)
#define HWIO_TURING_AON_PERF4_N_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF4_N_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF4_N_DFSR_ADDR, HWIO_TURING_AON_PERF4_N_DFSR_RMSK)
#define HWIO_TURING_AON_PERF4_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF4_N_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF4_N_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF4_N_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF4_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF4_N_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF4_N_DFSR_IN)
#define HWIO_TURING_AON_PERF4_N_DFSR_NOT_N_MINUS_M_BMSK                                                                 0xff
#define HWIO_TURING_AON_PERF4_N_DFSR_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_TURING_AON_PERF5_N_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x0000506c)
#define HWIO_TURING_AON_PERF5_N_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF5_N_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF5_N_DFSR_ADDR, HWIO_TURING_AON_PERF5_N_DFSR_RMSK)
#define HWIO_TURING_AON_PERF5_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF5_N_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF5_N_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF5_N_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF5_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF5_N_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF5_N_DFSR_IN)
#define HWIO_TURING_AON_PERF5_N_DFSR_NOT_N_MINUS_M_BMSK                                                                 0xff
#define HWIO_TURING_AON_PERF5_N_DFSR_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_TURING_AON_PERF6_N_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005070)
#define HWIO_TURING_AON_PERF6_N_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF6_N_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF6_N_DFSR_ADDR, HWIO_TURING_AON_PERF6_N_DFSR_RMSK)
#define HWIO_TURING_AON_PERF6_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF6_N_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF6_N_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF6_N_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF6_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF6_N_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF6_N_DFSR_IN)
#define HWIO_TURING_AON_PERF6_N_DFSR_NOT_N_MINUS_M_BMSK                                                                 0xff
#define HWIO_TURING_AON_PERF6_N_DFSR_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_TURING_AON_PERF7_N_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005074)
#define HWIO_TURING_AON_PERF7_N_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF7_N_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF7_N_DFSR_ADDR, HWIO_TURING_AON_PERF7_N_DFSR_RMSK)
#define HWIO_TURING_AON_PERF7_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF7_N_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF7_N_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF7_N_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF7_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF7_N_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF7_N_DFSR_IN)
#define HWIO_TURING_AON_PERF7_N_DFSR_NOT_N_MINUS_M_BMSK                                                                 0xff
#define HWIO_TURING_AON_PERF7_N_DFSR_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_TURING_AON_PERF0_D_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005078)
#define HWIO_TURING_AON_PERF0_D_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF0_D_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF0_D_DFSR_ADDR, HWIO_TURING_AON_PERF0_D_DFSR_RMSK)
#define HWIO_TURING_AON_PERF0_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF0_D_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF0_D_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF0_D_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF0_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF0_D_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF0_D_DFSR_IN)
#define HWIO_TURING_AON_PERF0_D_DFSR_NOT_2D_BMSK                                                                        0xff
#define HWIO_TURING_AON_PERF0_D_DFSR_NOT_2D_SHFT                                                                         0x0

#define HWIO_TURING_AON_PERF1_D_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x0000507c)
#define HWIO_TURING_AON_PERF1_D_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF1_D_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF1_D_DFSR_ADDR, HWIO_TURING_AON_PERF1_D_DFSR_RMSK)
#define HWIO_TURING_AON_PERF1_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF1_D_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF1_D_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF1_D_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF1_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF1_D_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF1_D_DFSR_IN)
#define HWIO_TURING_AON_PERF1_D_DFSR_NOT_2D_BMSK                                                                        0xff
#define HWIO_TURING_AON_PERF1_D_DFSR_NOT_2D_SHFT                                                                         0x0

#define HWIO_TURING_AON_PERF2_D_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005080)
#define HWIO_TURING_AON_PERF2_D_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF2_D_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF2_D_DFSR_ADDR, HWIO_TURING_AON_PERF2_D_DFSR_RMSK)
#define HWIO_TURING_AON_PERF2_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF2_D_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF2_D_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF2_D_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF2_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF2_D_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF2_D_DFSR_IN)
#define HWIO_TURING_AON_PERF2_D_DFSR_NOT_2D_BMSK                                                                        0xff
#define HWIO_TURING_AON_PERF2_D_DFSR_NOT_2D_SHFT                                                                         0x0

#define HWIO_TURING_AON_PERF3_D_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005084)
#define HWIO_TURING_AON_PERF3_D_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF3_D_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF3_D_DFSR_ADDR, HWIO_TURING_AON_PERF3_D_DFSR_RMSK)
#define HWIO_TURING_AON_PERF3_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF3_D_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF3_D_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF3_D_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF3_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF3_D_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF3_D_DFSR_IN)
#define HWIO_TURING_AON_PERF3_D_DFSR_NOT_2D_BMSK                                                                        0xff
#define HWIO_TURING_AON_PERF3_D_DFSR_NOT_2D_SHFT                                                                         0x0

#define HWIO_TURING_AON_PERF4_D_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005088)
#define HWIO_TURING_AON_PERF4_D_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF4_D_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF4_D_DFSR_ADDR, HWIO_TURING_AON_PERF4_D_DFSR_RMSK)
#define HWIO_TURING_AON_PERF4_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF4_D_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF4_D_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF4_D_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF4_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF4_D_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF4_D_DFSR_IN)
#define HWIO_TURING_AON_PERF4_D_DFSR_NOT_2D_BMSK                                                                        0xff
#define HWIO_TURING_AON_PERF4_D_DFSR_NOT_2D_SHFT                                                                         0x0

#define HWIO_TURING_AON_PERF5_D_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x0000508c)
#define HWIO_TURING_AON_PERF5_D_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF5_D_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF5_D_DFSR_ADDR, HWIO_TURING_AON_PERF5_D_DFSR_RMSK)
#define HWIO_TURING_AON_PERF5_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF5_D_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF5_D_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF5_D_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF5_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF5_D_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF5_D_DFSR_IN)
#define HWIO_TURING_AON_PERF5_D_DFSR_NOT_2D_BMSK                                                                        0xff
#define HWIO_TURING_AON_PERF5_D_DFSR_NOT_2D_SHFT                                                                         0x0

#define HWIO_TURING_AON_PERF6_D_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005090)
#define HWIO_TURING_AON_PERF6_D_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF6_D_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF6_D_DFSR_ADDR, HWIO_TURING_AON_PERF6_D_DFSR_RMSK)
#define HWIO_TURING_AON_PERF6_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF6_D_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF6_D_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF6_D_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF6_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF6_D_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF6_D_DFSR_IN)
#define HWIO_TURING_AON_PERF6_D_DFSR_NOT_2D_BMSK                                                                        0xff
#define HWIO_TURING_AON_PERF6_D_DFSR_NOT_2D_SHFT                                                                         0x0

#define HWIO_TURING_AON_PERF7_D_DFSR_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00005094)
#define HWIO_TURING_AON_PERF7_D_DFSR_RMSK                                                                               0xff
#define HWIO_TURING_AON_PERF7_D_DFSR_IN          \
        in_dword_masked(HWIO_TURING_AON_PERF7_D_DFSR_ADDR, HWIO_TURING_AON_PERF7_D_DFSR_RMSK)
#define HWIO_TURING_AON_PERF7_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_TURING_AON_PERF7_D_DFSR_ADDR, m)
#define HWIO_TURING_AON_PERF7_D_DFSR_OUT(v)      \
        out_dword(HWIO_TURING_AON_PERF7_D_DFSR_ADDR,v)
#define HWIO_TURING_AON_PERF7_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_AON_PERF7_D_DFSR_ADDR,m,v,HWIO_TURING_AON_PERF7_D_DFSR_IN)
#define HWIO_TURING_AON_PERF7_D_DFSR_NOT_2D_BMSK                                                                        0xff
#define HWIO_TURING_AON_PERF7_D_DFSR_NOT_2D_SHFT                                                                         0x0

#define HWIO_TURING_TURING_WRAPPER_AON_CBCR_ADDR                                                                  (TURING_CC_REG_BASE      + 0x00005098)
#define HWIO_TURING_TURING_WRAPPER_AON_CBCR_RMSK                                                                  0x80000003
#define HWIO_TURING_TURING_WRAPPER_AON_CBCR_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_AON_CBCR_ADDR, HWIO_TURING_TURING_WRAPPER_AON_CBCR_RMSK)
#define HWIO_TURING_TURING_WRAPPER_AON_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_AON_CBCR_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_AON_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_AON_CBCR_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_AON_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_AON_CBCR_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_AON_CBCR_IN)
#define HWIO_TURING_TURING_WRAPPER_AON_CBCR_CLK_OFF_BMSK                                                          0x80000000
#define HWIO_TURING_TURING_WRAPPER_AON_CBCR_CLK_OFF_SHFT                                                                0x1f
#define HWIO_TURING_TURING_WRAPPER_AON_CBCR_HW_CTL_BMSK                                                                  0x2
#define HWIO_TURING_TURING_WRAPPER_AON_CBCR_HW_CTL_SHFT                                                                  0x1
#define HWIO_TURING_TURING_WRAPPER_AON_CBCR_CLK_ENABLE_BMSK                                                              0x1
#define HWIO_TURING_TURING_WRAPPER_AON_CBCR_CLK_ENABLE_SHFT                                                              0x0

#define HWIO_TURING_SLEEP_CMD_RCGR_ADDR                                                                           (TURING_CC_REG_BASE      + 0x00006000)
#define HWIO_TURING_SLEEP_CMD_RCGR_RMSK                                                                           0x800000f3
#define HWIO_TURING_SLEEP_CMD_RCGR_IN          \
        in_dword_masked(HWIO_TURING_SLEEP_CMD_RCGR_ADDR, HWIO_TURING_SLEEP_CMD_RCGR_RMSK)
#define HWIO_TURING_SLEEP_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_TURING_SLEEP_CMD_RCGR_ADDR, m)
#define HWIO_TURING_SLEEP_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_TURING_SLEEP_CMD_RCGR_ADDR,v)
#define HWIO_TURING_SLEEP_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_SLEEP_CMD_RCGR_ADDR,m,v,HWIO_TURING_SLEEP_CMD_RCGR_IN)
#define HWIO_TURING_SLEEP_CMD_RCGR_ROOT_OFF_BMSK                                                                  0x80000000
#define HWIO_TURING_SLEEP_CMD_RCGR_ROOT_OFF_SHFT                                                                        0x1f
#define HWIO_TURING_SLEEP_CMD_RCGR_DIRTY_D_BMSK                                                                         0x80
#define HWIO_TURING_SLEEP_CMD_RCGR_DIRTY_D_SHFT                                                                          0x7
#define HWIO_TURING_SLEEP_CMD_RCGR_DIRTY_N_BMSK                                                                         0x40
#define HWIO_TURING_SLEEP_CMD_RCGR_DIRTY_N_SHFT                                                                          0x6
#define HWIO_TURING_SLEEP_CMD_RCGR_DIRTY_M_BMSK                                                                         0x20
#define HWIO_TURING_SLEEP_CMD_RCGR_DIRTY_M_SHFT                                                                          0x5
#define HWIO_TURING_SLEEP_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                                  0x10
#define HWIO_TURING_SLEEP_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                                   0x4
#define HWIO_TURING_SLEEP_CMD_RCGR_ROOT_EN_BMSK                                                                          0x2
#define HWIO_TURING_SLEEP_CMD_RCGR_ROOT_EN_SHFT                                                                          0x1
#define HWIO_TURING_SLEEP_CMD_RCGR_UPDATE_BMSK                                                                           0x1
#define HWIO_TURING_SLEEP_CMD_RCGR_UPDATE_SHFT                                                                           0x0

#define HWIO_TURING_SLEEP_CFG_RCGR_ADDR                                                                           (TURING_CC_REG_BASE      + 0x00006004)
#define HWIO_TURING_SLEEP_CFG_RCGR_RMSK                                                                              0x1371f
#define HWIO_TURING_SLEEP_CFG_RCGR_IN          \
        in_dword_masked(HWIO_TURING_SLEEP_CFG_RCGR_ADDR, HWIO_TURING_SLEEP_CFG_RCGR_RMSK)
#define HWIO_TURING_SLEEP_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_TURING_SLEEP_CFG_RCGR_ADDR, m)
#define HWIO_TURING_SLEEP_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_TURING_SLEEP_CFG_RCGR_ADDR,v)
#define HWIO_TURING_SLEEP_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_SLEEP_CFG_RCGR_ADDR,m,v,HWIO_TURING_SLEEP_CFG_RCGR_IN)
#define HWIO_TURING_SLEEP_CFG_RCGR_RCGLITE_DISABLE_BMSK                                                              0x10000
#define HWIO_TURING_SLEEP_CFG_RCGR_RCGLITE_DISABLE_SHFT                                                                 0x10
#define HWIO_TURING_SLEEP_CFG_RCGR_MODE_BMSK                                                                          0x3000
#define HWIO_TURING_SLEEP_CFG_RCGR_MODE_SHFT                                                                             0xc
#define HWIO_TURING_SLEEP_CFG_RCGR_SRC_SEL_BMSK                                                                        0x700
#define HWIO_TURING_SLEEP_CFG_RCGR_SRC_SEL_SHFT                                                                          0x8
#define HWIO_TURING_SLEEP_CFG_RCGR_SRC_DIV_BMSK                                                                         0x1f
#define HWIO_TURING_SLEEP_CFG_RCGR_SRC_DIV_SHFT                                                                          0x0

#define HWIO_TURING_XO_CMD_RCGR_ADDR                                                                              (TURING_CC_REG_BASE      + 0x00007000)
#define HWIO_TURING_XO_CMD_RCGR_RMSK                                                                              0x800000f3
#define HWIO_TURING_XO_CMD_RCGR_IN          \
        in_dword_masked(HWIO_TURING_XO_CMD_RCGR_ADDR, HWIO_TURING_XO_CMD_RCGR_RMSK)
#define HWIO_TURING_XO_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_TURING_XO_CMD_RCGR_ADDR, m)
#define HWIO_TURING_XO_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_TURING_XO_CMD_RCGR_ADDR,v)
#define HWIO_TURING_XO_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_XO_CMD_RCGR_ADDR,m,v,HWIO_TURING_XO_CMD_RCGR_IN)
#define HWIO_TURING_XO_CMD_RCGR_ROOT_OFF_BMSK                                                                     0x80000000
#define HWIO_TURING_XO_CMD_RCGR_ROOT_OFF_SHFT                                                                           0x1f
#define HWIO_TURING_XO_CMD_RCGR_DIRTY_D_BMSK                                                                            0x80
#define HWIO_TURING_XO_CMD_RCGR_DIRTY_D_SHFT                                                                             0x7
#define HWIO_TURING_XO_CMD_RCGR_DIRTY_N_BMSK                                                                            0x40
#define HWIO_TURING_XO_CMD_RCGR_DIRTY_N_SHFT                                                                             0x6
#define HWIO_TURING_XO_CMD_RCGR_DIRTY_M_BMSK                                                                            0x20
#define HWIO_TURING_XO_CMD_RCGR_DIRTY_M_SHFT                                                                             0x5
#define HWIO_TURING_XO_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                                     0x10
#define HWIO_TURING_XO_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                                      0x4
#define HWIO_TURING_XO_CMD_RCGR_ROOT_EN_BMSK                                                                             0x2
#define HWIO_TURING_XO_CMD_RCGR_ROOT_EN_SHFT                                                                             0x1
#define HWIO_TURING_XO_CMD_RCGR_UPDATE_BMSK                                                                              0x1
#define HWIO_TURING_XO_CMD_RCGR_UPDATE_SHFT                                                                              0x0

#define HWIO_TURING_XO_CFG_RCGR_ADDR                                                                              (TURING_CC_REG_BASE      + 0x00007004)
#define HWIO_TURING_XO_CFG_RCGR_RMSK                                                                                 0x1771f
#define HWIO_TURING_XO_CFG_RCGR_IN          \
        in_dword_masked(HWIO_TURING_XO_CFG_RCGR_ADDR, HWIO_TURING_XO_CFG_RCGR_RMSK)
#define HWIO_TURING_XO_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_TURING_XO_CFG_RCGR_ADDR, m)
#define HWIO_TURING_XO_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_TURING_XO_CFG_RCGR_ADDR,v)
#define HWIO_TURING_XO_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_XO_CFG_RCGR_ADDR,m,v,HWIO_TURING_XO_CFG_RCGR_IN)
#define HWIO_TURING_XO_CFG_RCGR_RCGLITE_DISABLE_BMSK                                                                 0x10000
#define HWIO_TURING_XO_CFG_RCGR_RCGLITE_DISABLE_SHFT                                                                    0x10
#define HWIO_TURING_XO_CFG_RCGR_ALT_SRC_SEL_BMSK                                                                      0x4000
#define HWIO_TURING_XO_CFG_RCGR_ALT_SRC_SEL_SHFT                                                                         0xe
#define HWIO_TURING_XO_CFG_RCGR_MODE_BMSK                                                                             0x3000
#define HWIO_TURING_XO_CFG_RCGR_MODE_SHFT                                                                                0xc
#define HWIO_TURING_XO_CFG_RCGR_SRC_SEL_BMSK                                                                           0x700
#define HWIO_TURING_XO_CFG_RCGR_SRC_SEL_SHFT                                                                             0x8
#define HWIO_TURING_XO_CFG_RCGR_SRC_DIV_BMSK                                                                            0x1f
#define HWIO_TURING_XO_CFG_RCGR_SRC_DIV_SHFT                                                                             0x0

#define HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_ADDR                                                        (TURING_CC_REG_BASE      + 0x00008000)
#define HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_RMSK                                                        0x80000003
#define HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_ADDR, HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_RMSK)
#define HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_IN)
#define HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_TURING_TURING_WRAPPER_CNOC_SWAY_AON_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_TURING_Q6SS_AHBM_AON_CBCR_ADDR                                                                       (TURING_CC_REG_BASE      + 0x00009000)
#define HWIO_TURING_Q6SS_AHBM_AON_CBCR_RMSK                                                                       0x80000003
#define HWIO_TURING_Q6SS_AHBM_AON_CBCR_IN          \
        in_dword_masked(HWIO_TURING_Q6SS_AHBM_AON_CBCR_ADDR, HWIO_TURING_Q6SS_AHBM_AON_CBCR_RMSK)
#define HWIO_TURING_Q6SS_AHBM_AON_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_Q6SS_AHBM_AON_CBCR_ADDR, m)
#define HWIO_TURING_Q6SS_AHBM_AON_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_Q6SS_AHBM_AON_CBCR_ADDR,v)
#define HWIO_TURING_Q6SS_AHBM_AON_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_Q6SS_AHBM_AON_CBCR_ADDR,m,v,HWIO_TURING_Q6SS_AHBM_AON_CBCR_IN)
#define HWIO_TURING_Q6SS_AHBM_AON_CBCR_CLK_OFF_BMSK                                                               0x80000000
#define HWIO_TURING_Q6SS_AHBM_AON_CBCR_CLK_OFF_SHFT                                                                     0x1f
#define HWIO_TURING_Q6SS_AHBM_AON_CBCR_HW_CTL_BMSK                                                                       0x2
#define HWIO_TURING_Q6SS_AHBM_AON_CBCR_HW_CTL_SHFT                                                                       0x1
#define HWIO_TURING_Q6SS_AHBM_AON_CBCR_CLK_ENABLE_BMSK                                                                   0x1
#define HWIO_TURING_Q6SS_AHBM_AON_CBCR_CLK_ENABLE_SHFT                                                                   0x0

#define HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_ADDR                                                            (TURING_CC_REG_BASE      + 0x0000a000)
#define HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_RMSK                                                            0x80000003
#define HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_ADDR, HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_RMSK)
#define HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_IN)
#define HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_HW_CTL_BMSK                                                            0x2
#define HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_HW_CTL_SHFT                                                            0x1
#define HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_TURING_TURING_WRAPPER_CNOC_AHBS_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_TURING_Q6SS_Q6_AXIM_CBCR_ADDR                                                                        (TURING_CC_REG_BASE      + 0x0000b000)
#define HWIO_TURING_Q6SS_Q6_AXIM_CBCR_RMSK                                                                        0x80000003
#define HWIO_TURING_Q6SS_Q6_AXIM_CBCR_IN          \
        in_dword_masked(HWIO_TURING_Q6SS_Q6_AXIM_CBCR_ADDR, HWIO_TURING_Q6SS_Q6_AXIM_CBCR_RMSK)
#define HWIO_TURING_Q6SS_Q6_AXIM_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_Q6SS_Q6_AXIM_CBCR_ADDR, m)
#define HWIO_TURING_Q6SS_Q6_AXIM_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_Q6SS_Q6_AXIM_CBCR_ADDR,v)
#define HWIO_TURING_Q6SS_Q6_AXIM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_Q6SS_Q6_AXIM_CBCR_ADDR,m,v,HWIO_TURING_Q6SS_Q6_AXIM_CBCR_IN)
#define HWIO_TURING_Q6SS_Q6_AXIM_CBCR_CLK_OFF_BMSK                                                                0x80000000
#define HWIO_TURING_Q6SS_Q6_AXIM_CBCR_CLK_OFF_SHFT                                                                      0x1f
#define HWIO_TURING_Q6SS_Q6_AXIM_CBCR_HW_CTL_BMSK                                                                        0x2
#define HWIO_TURING_Q6SS_Q6_AXIM_CBCR_HW_CTL_SHFT                                                                        0x1
#define HWIO_TURING_Q6SS_Q6_AXIM_CBCR_CLK_ENABLE_BMSK                                                                    0x1
#define HWIO_TURING_Q6SS_Q6_AXIM_CBCR_CLK_ENABLE_SHFT                                                                    0x0

#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_ADDR                                                       (TURING_CC_REG_BASE      + 0x0000c000)
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_RMSK                                                       0x80000003
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_IN          \
        in_dword_masked(HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_ADDR, HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_RMSK)
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_ADDR, m)
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_ADDR,v)
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_ADDR,m,v,HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_IN)
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_HW_CTL_BMSK                                                       0x2
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_HW_CTL_SHFT                                                       0x1
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_0_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_ADDR                                                       (TURING_CC_REG_BASE      + 0x0000d000)
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_RMSK                                                       0x80000003
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_IN          \
        in_dword_masked(HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_ADDR, HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_RMSK)
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_ADDR, m)
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_ADDR,v)
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_ADDR,m,v,HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_IN)
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_HW_CTL_BMSK                                                       0x2
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_HW_CTL_SHFT                                                       0x1
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_TURING_Q6SS_Q6_CAMSS_DSP_STREAMING_1_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_TURING_Q6SS_AHBS_AON_CBCR_ADDR                                                                       (TURING_CC_REG_BASE      + 0x00010000)
#define HWIO_TURING_Q6SS_AHBS_AON_CBCR_RMSK                                                                       0x80000003
#define HWIO_TURING_Q6SS_AHBS_AON_CBCR_IN          \
        in_dword_masked(HWIO_TURING_Q6SS_AHBS_AON_CBCR_ADDR, HWIO_TURING_Q6SS_AHBS_AON_CBCR_RMSK)
#define HWIO_TURING_Q6SS_AHBS_AON_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_Q6SS_AHBS_AON_CBCR_ADDR, m)
#define HWIO_TURING_Q6SS_AHBS_AON_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_Q6SS_AHBS_AON_CBCR_ADDR,v)
#define HWIO_TURING_Q6SS_AHBS_AON_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_Q6SS_AHBS_AON_CBCR_ADDR,m,v,HWIO_TURING_Q6SS_AHBS_AON_CBCR_IN)
#define HWIO_TURING_Q6SS_AHBS_AON_CBCR_CLK_OFF_BMSK                                                               0x80000000
#define HWIO_TURING_Q6SS_AHBS_AON_CBCR_CLK_OFF_SHFT                                                                     0x1f
#define HWIO_TURING_Q6SS_AHBS_AON_CBCR_HW_CTL_BMSK                                                                       0x2
#define HWIO_TURING_Q6SS_AHBS_AON_CBCR_HW_CTL_SHFT                                                                       0x1
#define HWIO_TURING_Q6SS_AHBS_AON_CBCR_CLK_ENABLE_BMSK                                                                   0x1
#define HWIO_TURING_Q6SS_AHBS_AON_CBCR_CLK_ENABLE_SHFT                                                                   0x0

#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_ADDR                                                           (TURING_CC_REG_BASE      + 0x00011000)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_RMSK                                                           0x800000f3
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_IN          \
        in_dword_masked(HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_ADDR, HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_RMSK)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_ADDR, m)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_ADDR,v)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_ADDR,m,v,HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_IN)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_ROOT_OFF_BMSK                                                  0x80000000
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_ROOT_OFF_SHFT                                                        0x1f
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_DIRTY_D_BMSK                                                         0x80
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_DIRTY_D_SHFT                                                          0x7
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_DIRTY_N_BMSK                                                         0x40
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_DIRTY_N_SHFT                                                          0x6
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_DIRTY_M_BMSK                                                         0x20
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_DIRTY_M_SHFT                                                          0x5
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                  0x10
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                   0x4
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_ROOT_EN_BMSK                                                          0x2
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_ROOT_EN_SHFT                                                          0x1
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_UPDATE_BMSK                                                           0x1
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CMD_RCGR_UPDATE_SHFT                                                           0x0

#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_ADDR                                                           (TURING_CC_REG_BASE      + 0x00011004)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_RMSK                                                               0x771f
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_IN          \
        in_dword_masked(HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_ADDR, HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_RMSK)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_ADDR, m)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_ADDR,v)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_ADDR,m,v,HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_IN)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_ALT_SRC_SEL_BMSK                                                   0x4000
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_ALT_SRC_SEL_SHFT                                                      0xe
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_MODE_BMSK                                                          0x3000
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_MODE_SHFT                                                             0xc
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_SRC_SEL_BMSK                                                        0x700
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_SRC_SEL_SHFT                                                          0x8
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_SRC_DIV_BMSK                                                         0x1f
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_CFG_RCGR_SRC_DIV_SHFT                                                          0x0

#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_M_ADDR                                                                  (TURING_CC_REG_BASE      + 0x00011008)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_M_RMSK                                                                        0xff
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_M_IN          \
        in_dword_masked(HWIO_TURING_QOS_FIXED_LAT_COUNTER_M_ADDR, HWIO_TURING_QOS_FIXED_LAT_COUNTER_M_RMSK)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_M_INM(m)      \
        in_dword_masked(HWIO_TURING_QOS_FIXED_LAT_COUNTER_M_ADDR, m)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_M_OUT(v)      \
        out_dword(HWIO_TURING_QOS_FIXED_LAT_COUNTER_M_ADDR,v)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QOS_FIXED_LAT_COUNTER_M_ADDR,m,v,HWIO_TURING_QOS_FIXED_LAT_COUNTER_M_IN)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_M_M_BMSK                                                                      0xff
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_M_M_SHFT                                                                       0x0

#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_N_ADDR                                                                  (TURING_CC_REG_BASE      + 0x0001100c)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_N_RMSK                                                                        0xff
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_N_IN          \
        in_dword_masked(HWIO_TURING_QOS_FIXED_LAT_COUNTER_N_ADDR, HWIO_TURING_QOS_FIXED_LAT_COUNTER_N_RMSK)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_N_INM(m)      \
        in_dword_masked(HWIO_TURING_QOS_FIXED_LAT_COUNTER_N_ADDR, m)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_N_OUT(v)      \
        out_dword(HWIO_TURING_QOS_FIXED_LAT_COUNTER_N_ADDR,v)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QOS_FIXED_LAT_COUNTER_N_ADDR,m,v,HWIO_TURING_QOS_FIXED_LAT_COUNTER_N_IN)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_N_NOT_N_MINUS_M_BMSK                                                          0xff
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_N_NOT_N_MINUS_M_SHFT                                                           0x0

#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_D_ADDR                                                                  (TURING_CC_REG_BASE      + 0x00011010)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_D_RMSK                                                                        0xff
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_D_IN          \
        in_dword_masked(HWIO_TURING_QOS_FIXED_LAT_COUNTER_D_ADDR, HWIO_TURING_QOS_FIXED_LAT_COUNTER_D_RMSK)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_D_INM(m)      \
        in_dword_masked(HWIO_TURING_QOS_FIXED_LAT_COUNTER_D_ADDR, m)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_D_OUT(v)      \
        out_dword(HWIO_TURING_QOS_FIXED_LAT_COUNTER_D_ADDR,v)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QOS_FIXED_LAT_COUNTER_D_ADDR,m,v,HWIO_TURING_QOS_FIXED_LAT_COUNTER_D_IN)
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_D_NOT_2D_BMSK                                                                 0xff
#define HWIO_TURING_QOS_FIXED_LAT_COUNTER_D_NOT_2D_SHFT                                                                  0x0

#define HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_ADDR                                                         (TURING_CC_REG_BASE      + 0x00011014)
#define HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_RMSK                                                         0x80000003
#define HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_ADDR, HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_RMSK)
#define HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_IN)
#define HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_HW_CTL_BMSK                                                         0x2
#define HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_HW_CTL_SHFT                                                         0x1
#define HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_TURING_TURING_WRAPPER_QOS_AHBS_AON_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_ADDR                                                   (TURING_CC_REG_BASE      + 0x00011018)
#define HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_RMSK                                                   0x80000003
#define HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_ADDR, HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_RMSK)
#define HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_IN)
#define HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_CLK_OFF_BMSK                                           0x80000000
#define HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_CLK_OFF_SHFT                                                 0x1f
#define HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_HW_CTL_BMSK                                                   0x2
#define HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_HW_CTL_SHFT                                                   0x1
#define HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_CLK_ENABLE_BMSK                                               0x1
#define HWIO_TURING_TURING_WRAPPER_QOS_XO_LAT_COUNTER_CBCR_CLK_ENABLE_SHFT                                               0x0

#define HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_ADDR                                       (TURING_CC_REG_BASE      + 0x0001101c)
#define HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_RMSK                                       0x80000003
#define HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_ADDR, HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_RMSK)
#define HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_IN)
#define HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_CLK_OFF_BMSK                               0x80000000
#define HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_CLK_OFF_SHFT                                     0x1f
#define HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_HW_CTL_BMSK                                       0x2
#define HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_HW_CTL_SHFT                                       0x1
#define HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_CLK_ENABLE_BMSK                                   0x1
#define HWIO_TURING_TURING_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR_CLK_ENABLE_SHFT                                   0x0

#define HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_ADDR                                         (TURING_CC_REG_BASE      + 0x00011020)
#define HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_RMSK                                         0x80000003
#define HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_ADDR, HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_RMSK)
#define HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_IN)
#define HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_CLK_OFF_BMSK                                 0x80000000
#define HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_CLK_OFF_SHFT                                       0x1f
#define HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_HW_CTL_BMSK                                         0x2
#define HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_HW_CTL_SHFT                                         0x1
#define HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_CLK_ENABLE_BMSK                                     0x1
#define HWIO_TURING_TURING_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR_CLK_ENABLE_SHFT                                     0x0

#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_Q6_ADDR                                                 (TURING_CC_REG_BASE      + 0x00012004)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_Q6_RMSK                                                        0x1
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_Q6_IN          \
        in_dword_masked(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_Q6_ADDR, HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_Q6_RMSK)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_Q6_INM(m)      \
        in_dword_masked(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_Q6_ADDR, m)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_Q6_OUT(v)      \
        out_dword(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_Q6_ADDR,v)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_Q6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_Q6_ADDR,m,v,HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_Q6_IN)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_Q6_VAPSS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_BMSK                   0x1
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_Q6_VAPSS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_SHFT                   0x0

#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_ADDR                                                 (TURING_CC_REG_BASE      + 0x00012008)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_RMSK                                                        0x1
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_IN          \
        in_dword_masked(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_ADDR, HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_RMSK)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_INM(m)      \
        in_dword_masked(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_ADDR, m)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_OUT(v)      \
        out_dword(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_ADDR,v)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_ADDR,m,v,HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_IN)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_VAPSS_SEQUENCE_ABORT_IRQ_EN_BMSK                            0x1
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_VAPSS_SEQUENCE_ABORT_IRQ_EN_SHFT                            0x0

#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_APPS_ADDR                                               (TURING_CC_REG_BASE      + 0x00013004)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_APPS_RMSK                                                      0x1
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_APPS_IN          \
        in_dword_masked(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_APPS_ADDR, HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_APPS_RMSK)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_APPS_INM(m)      \
        in_dword_masked(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_APPS_ADDR, m)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_APPS_OUT(v)      \
        out_dword(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_APPS_ADDR,v)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_APPS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_APPS_ADDR,m,v,HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_APPS_IN)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_APPS_VAPSS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_BMSK                 0x1
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_STATUS_APPS_VAPSS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_SHFT                 0x0

#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_ADDR                                               (TURING_CC_REG_BASE      + 0x00013008)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_RMSK                                                      0x1
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_IN          \
        in_dword_masked(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_ADDR, HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_RMSK)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_INM(m)      \
        in_dword_masked(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_ADDR, m)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_OUT(v)      \
        out_dword(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_ADDR,v)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_ADDR,m,v,HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_IN)
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_VAPSS_SEQUENCE_ABORT_IRQ_EN_BMSK                          0x1
#define HWIO_TURING_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_VAPSS_SEQUENCE_ABORT_IRQ_EN_SHFT                          0x0

#define HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_ADDR                                                      (TURING_CC_REG_BASE      + 0x00014000)
#define HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_RMSK                                                      0x80000003
#define HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_ADDR, HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_RMSK)
#define HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_IN)
#define HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_TURING_TURING_WRAPPER_BUS_TIMEOUT_AON_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_TURING_JBIST_PLL_SRC_SEL_ADDR                                                                        (TURING_CC_REG_BASE      + 0x00015000)
#define HWIO_TURING_JBIST_PLL_SRC_SEL_RMSK                                                                               0x3
#define HWIO_TURING_JBIST_PLL_SRC_SEL_IN          \
        in_dword_masked(HWIO_TURING_JBIST_PLL_SRC_SEL_ADDR, HWIO_TURING_JBIST_PLL_SRC_SEL_RMSK)
#define HWIO_TURING_JBIST_PLL_SRC_SEL_INM(m)      \
        in_dword_masked(HWIO_TURING_JBIST_PLL_SRC_SEL_ADDR, m)
#define HWIO_TURING_JBIST_PLL_SRC_SEL_OUT(v)      \
        out_dword(HWIO_TURING_JBIST_PLL_SRC_SEL_ADDR,v)
#define HWIO_TURING_JBIST_PLL_SRC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_JBIST_PLL_SRC_SEL_ADDR,m,v,HWIO_TURING_JBIST_PLL_SRC_SEL_IN)
#define HWIO_TURING_JBIST_PLL_SRC_SEL_JBIST_PLL_CLK_SRC_SEL_BMSK                                                         0x3
#define HWIO_TURING_JBIST_PLL_SRC_SEL_JBIST_PLL_CLK_SRC_SEL_SHFT                                                         0x0

#define HWIO_TURING_JBIST_MODE_ADDR                                                                               (TURING_CC_REG_BASE      + 0x00015004)
#define HWIO_TURING_JBIST_MODE_RMSK                                                                               0xffffffff
#define HWIO_TURING_JBIST_MODE_IN          \
        in_dword_masked(HWIO_TURING_JBIST_MODE_ADDR, HWIO_TURING_JBIST_MODE_RMSK)
#define HWIO_TURING_JBIST_MODE_INM(m)      \
        in_dword_masked(HWIO_TURING_JBIST_MODE_ADDR, m)
#define HWIO_TURING_JBIST_MODE_OUT(v)      \
        out_dword(HWIO_TURING_JBIST_MODE_ADDR,v)
#define HWIO_TURING_JBIST_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_JBIST_MODE_ADDR,m,v,HWIO_TURING_JBIST_MODE_IN)
#define HWIO_TURING_JBIST_MODE_RESERVE_BITS31_4_BMSK                                                              0xfffffff0
#define HWIO_TURING_JBIST_MODE_RESERVE_BITS31_4_SHFT                                                                     0x4
#define HWIO_TURING_JBIST_MODE_START_MEAS_BMSK                                                                           0x8
#define HWIO_TURING_JBIST_MODE_START_MEAS_SHFT                                                                           0x3
#define HWIO_TURING_JBIST_MODE_JBIST_TEST_BMSK                                                                           0x4
#define HWIO_TURING_JBIST_MODE_JBIST_TEST_SHFT                                                                           0x2
#define HWIO_TURING_JBIST_MODE_RESET_N_BMSK                                                                              0x2
#define HWIO_TURING_JBIST_MODE_RESET_N_SHFT                                                                              0x1
#define HWIO_TURING_JBIST_MODE_SLEEP_N_BMSK                                                                              0x1
#define HWIO_TURING_JBIST_MODE_SLEEP_N_SHFT                                                                              0x0

#define HWIO_TURING_JBIST_CONFIG_CTL_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00015008)
#define HWIO_TURING_JBIST_CONFIG_CTL_RMSK                                                                         0xffffffff
#define HWIO_TURING_JBIST_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_TURING_JBIST_CONFIG_CTL_ADDR, HWIO_TURING_JBIST_CONFIG_CTL_RMSK)
#define HWIO_TURING_JBIST_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_TURING_JBIST_CONFIG_CTL_ADDR, m)
#define HWIO_TURING_JBIST_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_TURING_JBIST_CONFIG_CTL_ADDR,v)
#define HWIO_TURING_JBIST_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_JBIST_CONFIG_CTL_ADDR,m,v,HWIO_TURING_JBIST_CONFIG_CTL_IN)
#define HWIO_TURING_JBIST_CONFIG_CTL_JBIST_CONFIG_CTL_BMSK                                                        0xffffffff
#define HWIO_TURING_JBIST_CONFIG_CTL_JBIST_CONFIG_CTL_SHFT                                                               0x0

#define HWIO_TURING_JBIST_USER_CTL_ADDR                                                                           (TURING_CC_REG_BASE      + 0x0001500c)
#define HWIO_TURING_JBIST_USER_CTL_RMSK                                                                           0xffffffff
#define HWIO_TURING_JBIST_USER_CTL_IN          \
        in_dword_masked(HWIO_TURING_JBIST_USER_CTL_ADDR, HWIO_TURING_JBIST_USER_CTL_RMSK)
#define HWIO_TURING_JBIST_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_TURING_JBIST_USER_CTL_ADDR, m)
#define HWIO_TURING_JBIST_USER_CTL_OUT(v)      \
        out_dword(HWIO_TURING_JBIST_USER_CTL_ADDR,v)
#define HWIO_TURING_JBIST_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_JBIST_USER_CTL_ADDR,m,v,HWIO_TURING_JBIST_USER_CTL_IN)
#define HWIO_TURING_JBIST_USER_CTL_JBIST_USER_CTL_BMSK                                                            0xffffffff
#define HWIO_TURING_JBIST_USER_CTL_JBIST_USER_CTL_SHFT                                                                   0x0

#define HWIO_TURING_JBIST_USER_CTL_U_ADDR                                                                         (TURING_CC_REG_BASE      + 0x00015010)
#define HWIO_TURING_JBIST_USER_CTL_U_RMSK                                                                         0xffffffff
#define HWIO_TURING_JBIST_USER_CTL_U_IN          \
        in_dword_masked(HWIO_TURING_JBIST_USER_CTL_U_ADDR, HWIO_TURING_JBIST_USER_CTL_U_RMSK)
#define HWIO_TURING_JBIST_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_TURING_JBIST_USER_CTL_U_ADDR, m)
#define HWIO_TURING_JBIST_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_TURING_JBIST_USER_CTL_U_ADDR,v)
#define HWIO_TURING_JBIST_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_JBIST_USER_CTL_U_ADDR,m,v,HWIO_TURING_JBIST_USER_CTL_U_IN)
#define HWIO_TURING_JBIST_USER_CTL_U_JBIST_USER_CTL_U_BMSK                                                        0xffffffff
#define HWIO_TURING_JBIST_USER_CTL_U_JBIST_USER_CTL_U_SHFT                                                               0x0

#define HWIO_TURING_JBIST_TEST_CTL_ADDR                                                                           (TURING_CC_REG_BASE      + 0x00015014)
#define HWIO_TURING_JBIST_TEST_CTL_RMSK                                                                           0xffffffff
#define HWIO_TURING_JBIST_TEST_CTL_IN          \
        in_dword_masked(HWIO_TURING_JBIST_TEST_CTL_ADDR, HWIO_TURING_JBIST_TEST_CTL_RMSK)
#define HWIO_TURING_JBIST_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_TURING_JBIST_TEST_CTL_ADDR, m)
#define HWIO_TURING_JBIST_TEST_CTL_OUT(v)      \
        out_dword(HWIO_TURING_JBIST_TEST_CTL_ADDR,v)
#define HWIO_TURING_JBIST_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_JBIST_TEST_CTL_ADDR,m,v,HWIO_TURING_JBIST_TEST_CTL_IN)
#define HWIO_TURING_JBIST_TEST_CTL_JBIST_TEST_CTL_BMSK                                                            0xffffffff
#define HWIO_TURING_JBIST_TEST_CTL_JBIST_TEST_CTL_SHFT                                                                   0x0

#define HWIO_TURING_JBIST_STATUS_ADDR                                                                             (TURING_CC_REG_BASE      + 0x00015018)
#define HWIO_TURING_JBIST_STATUS_RMSK                                                                             0xffffffff
#define HWIO_TURING_JBIST_STATUS_IN          \
        in_dword_masked(HWIO_TURING_JBIST_STATUS_ADDR, HWIO_TURING_JBIST_STATUS_RMSK)
#define HWIO_TURING_JBIST_STATUS_INM(m)      \
        in_dword_masked(HWIO_TURING_JBIST_STATUS_ADDR, m)
#define HWIO_TURING_JBIST_STATUS_JBIST_STATUS_BMSK                                                                0xffffffff
#define HWIO_TURING_JBIST_STATUS_JBIST_STATUS_SHFT                                                                       0x0

#define HWIO_TURING_JBIST_MEAS_DONE_ADDR                                                                          (TURING_CC_REG_BASE      + 0x0001501c)
#define HWIO_TURING_JBIST_MEAS_DONE_RMSK                                                                          0xffffffff
#define HWIO_TURING_JBIST_MEAS_DONE_IN          \
        in_dword_masked(HWIO_TURING_JBIST_MEAS_DONE_ADDR, HWIO_TURING_JBIST_MEAS_DONE_RMSK)
#define HWIO_TURING_JBIST_MEAS_DONE_INM(m)      \
        in_dword_masked(HWIO_TURING_JBIST_MEAS_DONE_ADDR, m)
#define HWIO_TURING_JBIST_MEAS_DONE_OUT(v)      \
        out_dword(HWIO_TURING_JBIST_MEAS_DONE_ADDR,v)
#define HWIO_TURING_JBIST_MEAS_DONE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_JBIST_MEAS_DONE_ADDR,m,v,HWIO_TURING_JBIST_MEAS_DONE_IN)
#define HWIO_TURING_JBIST_MEAS_DONE_RESERVE_BITS31_1_BMSK                                                         0xfffffffe
#define HWIO_TURING_JBIST_MEAS_DONE_RESERVE_BITS31_1_SHFT                                                                0x1
#define HWIO_TURING_JBIST_MEAS_DONE_JBIST_MEAS_DONE_BMSK                                                                 0x1
#define HWIO_TURING_JBIST_MEAS_DONE_JBIST_MEAS_DONE_SHFT                                                                 0x0

#define HWIO_TURING_VAPSS_GDSCR_ADDR                                                                              (TURING_CC_REG_BASE      + 0x00016000)
#define HWIO_TURING_VAPSS_GDSCR_RMSK                                                                              0xf8ffffff
#define HWIO_TURING_VAPSS_GDSCR_IN          \
        in_dword_masked(HWIO_TURING_VAPSS_GDSCR_ADDR, HWIO_TURING_VAPSS_GDSCR_RMSK)
#define HWIO_TURING_VAPSS_GDSCR_INM(m)      \
        in_dword_masked(HWIO_TURING_VAPSS_GDSCR_ADDR, m)
#define HWIO_TURING_VAPSS_GDSCR_OUT(v)      \
        out_dword(HWIO_TURING_VAPSS_GDSCR_ADDR,v)
#define HWIO_TURING_VAPSS_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAPSS_GDSCR_ADDR,m,v,HWIO_TURING_VAPSS_GDSCR_IN)
#define HWIO_TURING_VAPSS_GDSCR_PWR_ON_BMSK                                                                       0x80000000
#define HWIO_TURING_VAPSS_GDSCR_PWR_ON_SHFT                                                                             0x1f
#define HWIO_TURING_VAPSS_GDSCR_GDSC_STATE_BMSK                                                                   0x78000000
#define HWIO_TURING_VAPSS_GDSCR_GDSC_STATE_SHFT                                                                         0x1b
#define HWIO_TURING_VAPSS_GDSCR_EN_REST_WAIT_BMSK                                                                   0xf00000
#define HWIO_TURING_VAPSS_GDSCR_EN_REST_WAIT_SHFT                                                                       0x14
#define HWIO_TURING_VAPSS_GDSCR_EN_FEW_WAIT_BMSK                                                                     0xf0000
#define HWIO_TURING_VAPSS_GDSCR_EN_FEW_WAIT_SHFT                                                                        0x10
#define HWIO_TURING_VAPSS_GDSCR_CLK_DIS_WAIT_BMSK                                                                     0xf000
#define HWIO_TURING_VAPSS_GDSCR_CLK_DIS_WAIT_SHFT                                                                        0xc
#define HWIO_TURING_VAPSS_GDSCR_RETAIN_FF_ENABLE_BMSK                                                                  0x800
#define HWIO_TURING_VAPSS_GDSCR_RETAIN_FF_ENABLE_SHFT                                                                    0xb
#define HWIO_TURING_VAPSS_GDSCR_RESTORE_BMSK                                                                           0x400
#define HWIO_TURING_VAPSS_GDSCR_RESTORE_SHFT                                                                             0xa
#define HWIO_TURING_VAPSS_GDSCR_SAVE_BMSK                                                                              0x200
#define HWIO_TURING_VAPSS_GDSCR_SAVE_SHFT                                                                                0x9
#define HWIO_TURING_VAPSS_GDSCR_RETAIN_BMSK                                                                            0x100
#define HWIO_TURING_VAPSS_GDSCR_RETAIN_SHFT                                                                              0x8
#define HWIO_TURING_VAPSS_GDSCR_EN_REST_BMSK                                                                            0x80
#define HWIO_TURING_VAPSS_GDSCR_EN_REST_SHFT                                                                             0x7
#define HWIO_TURING_VAPSS_GDSCR_EN_FEW_BMSK                                                                             0x40
#define HWIO_TURING_VAPSS_GDSCR_EN_FEW_SHFT                                                                              0x6
#define HWIO_TURING_VAPSS_GDSCR_CLAMP_IO_BMSK                                                                           0x20
#define HWIO_TURING_VAPSS_GDSCR_CLAMP_IO_SHFT                                                                            0x5
#define HWIO_TURING_VAPSS_GDSCR_CLK_DISABLE_BMSK                                                                        0x10
#define HWIO_TURING_VAPSS_GDSCR_CLK_DISABLE_SHFT                                                                         0x4
#define HWIO_TURING_VAPSS_GDSCR_PD_ARES_BMSK                                                                             0x8
#define HWIO_TURING_VAPSS_GDSCR_PD_ARES_SHFT                                                                             0x3
#define HWIO_TURING_VAPSS_GDSCR_SW_OVERRIDE_BMSK                                                                         0x4
#define HWIO_TURING_VAPSS_GDSCR_SW_OVERRIDE_SHFT                                                                         0x2
#define HWIO_TURING_VAPSS_GDSCR_HW_CONTROL_BMSK                                                                          0x2
#define HWIO_TURING_VAPSS_GDSCR_HW_CONTROL_SHFT                                                                          0x1
#define HWIO_TURING_VAPSS_GDSCR_SW_COLLAPSE_BMSK                                                                         0x1
#define HWIO_TURING_VAPSS_GDSCR_SW_COLLAPSE_SHFT                                                                         0x0

#define HWIO_TURING_VAPSS_CFG_GDSCR_ADDR                                                                          (TURING_CC_REG_BASE      + 0x00016004)
#define HWIO_TURING_VAPSS_CFG_GDSCR_RMSK                                                                          0xffffffff
#define HWIO_TURING_VAPSS_CFG_GDSCR_IN          \
        in_dword_masked(HWIO_TURING_VAPSS_CFG_GDSCR_ADDR, HWIO_TURING_VAPSS_CFG_GDSCR_RMSK)
#define HWIO_TURING_VAPSS_CFG_GDSCR_INM(m)      \
        in_dword_masked(HWIO_TURING_VAPSS_CFG_GDSCR_ADDR, m)
#define HWIO_TURING_VAPSS_CFG_GDSCR_OUT(v)      \
        out_dword(HWIO_TURING_VAPSS_CFG_GDSCR_ADDR,v)
#define HWIO_TURING_VAPSS_CFG_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAPSS_CFG_GDSCR_ADDR,m,v,HWIO_TURING_VAPSS_CFG_GDSCR_IN)
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_SPARE_CTRL_IN_BMSK                                                       0xf0000000
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_SPARE_CTRL_IN_SHFT                                                             0x1c
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_SPARE_CTRL_OUT_BMSK                                                       0xc000000
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_SPARE_CTRL_OUT_SHFT                                                            0x1a
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_PWR_UP_START_BMSK                                                         0x2000000
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_PWR_UP_START_SHFT                                                              0x19
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_PWR_DWN_START_BMSK                                                        0x1000000
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_PWR_DWN_START_SHFT                                                             0x18
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_CFG_FSM_STATE_STATUS_BMSK                                                  0xf00000
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_CFG_FSM_STATE_STATUS_SHFT                                                      0x14
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_MEM_PWR_ACK_STATUS_BMSK                                                     0x80000
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_MEM_PWR_ACK_STATUS_SHFT                                                        0x13
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_ENR_ACK_STATUS_BMSK                                                         0x40000
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_ENR_ACK_STATUS_SHFT                                                            0x12
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_ENF_ACK_STATUS_BMSK                                                         0x20000
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_ENF_ACK_STATUS_SHFT                                                            0x11
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_POWER_UP_COMPLETE_BMSK                                                      0x10000
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_POWER_UP_COMPLETE_SHFT                                                         0x10
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_POWER_DOWN_COMPLETE_BMSK                                                     0x8000
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_POWER_DOWN_COMPLETE_SHFT                                                        0xf
#define HWIO_TURING_VAPSS_CFG_GDSCR_SOFTWARE_CONTROL_OVERRIDE_BMSK                                                    0x7800
#define HWIO_TURING_VAPSS_CFG_GDSCR_SOFTWARE_CONTROL_OVERRIDE_SHFT                                                       0xb
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_HANDSHAKE_DIS_BMSK                                                            0x400
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_HANDSHAKE_DIS_SHFT                                                              0xa
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_MEM_PERI_FORCE_IN_SW_BMSK                                                     0x200
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_MEM_PERI_FORCE_IN_SW_SHFT                                                       0x9
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_MEM_CORE_FORCE_IN_SW_BMSK                                                     0x100
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_MEM_CORE_FORCE_IN_SW_SHFT                                                       0x8
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_PHASE_RESET_EN_SW_BMSK                                                         0x80
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_PHASE_RESET_EN_SW_SHFT                                                          0x7
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_PHASE_RESET_DELAY_COUNT_SW_BMSK                                                0x60
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_PHASE_RESET_DELAY_COUNT_SW_SHFT                                                 0x5
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_PSCBC_PWR_DWN_SW_BMSK                                                          0x10
#define HWIO_TURING_VAPSS_CFG_GDSCR_GDSC_PSCBC_PWR_DWN_SW_SHFT                                                           0x4
#define HWIO_TURING_VAPSS_CFG_GDSCR_UNCLAMP_IO_SOFTWARE_OVERRIDE_BMSK                                                    0x8
#define HWIO_TURING_VAPSS_CFG_GDSCR_UNCLAMP_IO_SOFTWARE_OVERRIDE_SHFT                                                    0x3
#define HWIO_TURING_VAPSS_CFG_GDSCR_SAVE_RESTORE_SOFTWARE_OVERRIDE_BMSK                                                  0x4
#define HWIO_TURING_VAPSS_CFG_GDSCR_SAVE_RESTORE_SOFTWARE_OVERRIDE_SHFT                                                  0x2
#define HWIO_TURING_VAPSS_CFG_GDSCR_CLAMP_IO_SOFTWARE_OVERRIDE_BMSK                                                      0x2
#define HWIO_TURING_VAPSS_CFG_GDSCR_CLAMP_IO_SOFTWARE_OVERRIDE_SHFT                                                      0x1
#define HWIO_TURING_VAPSS_CFG_GDSCR_DISABLE_CLK_SOFTWARE_OVERRIDE_BMSK                                                   0x1
#define HWIO_TURING_VAPSS_CFG_GDSCR_DISABLE_CLK_SOFTWARE_OVERRIDE_SHFT                                                   0x0

#define HWIO_TURING_VAPSS_GDSC_XO_CBCR_ADDR                                                                       (TURING_CC_REG_BASE      + 0x00016008)
#define HWIO_TURING_VAPSS_GDSC_XO_CBCR_RMSK                                                                       0x80000003
#define HWIO_TURING_VAPSS_GDSC_XO_CBCR_IN          \
        in_dword_masked(HWIO_TURING_VAPSS_GDSC_XO_CBCR_ADDR, HWIO_TURING_VAPSS_GDSC_XO_CBCR_RMSK)
#define HWIO_TURING_VAPSS_GDSC_XO_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_VAPSS_GDSC_XO_CBCR_ADDR, m)
#define HWIO_TURING_VAPSS_GDSC_XO_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_VAPSS_GDSC_XO_CBCR_ADDR,v)
#define HWIO_TURING_VAPSS_GDSC_XO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAPSS_GDSC_XO_CBCR_ADDR,m,v,HWIO_TURING_VAPSS_GDSC_XO_CBCR_IN)
#define HWIO_TURING_VAPSS_GDSC_XO_CBCR_CLK_OFF_BMSK                                                               0x80000000
#define HWIO_TURING_VAPSS_GDSC_XO_CBCR_CLK_OFF_SHFT                                                                     0x1f
#define HWIO_TURING_VAPSS_GDSC_XO_CBCR_HW_CTL_BMSK                                                                       0x2
#define HWIO_TURING_VAPSS_GDSC_XO_CBCR_HW_CTL_SHFT                                                                       0x1
#define HWIO_TURING_VAPSS_GDSC_XO_CBCR_CLK_ENABLE_BMSK                                                                   0x1
#define HWIO_TURING_VAPSS_GDSC_XO_CBCR_CLK_ENABLE_SHFT                                                                   0x0

#define HWIO_TURING_VAPSS_GDS_HW_CTRL_ADDR                                                                        (TURING_CC_REG_BASE      + 0x0001600c)
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_RMSK                                                                        0xffffffff
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_IN          \
        in_dword_masked(HWIO_TURING_VAPSS_GDS_HW_CTRL_ADDR, HWIO_TURING_VAPSS_GDS_HW_CTRL_RMSK)
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_INM(m)      \
        in_dword_masked(HWIO_TURING_VAPSS_GDS_HW_CTRL_ADDR, m)
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_OUT(v)      \
        out_dword(HWIO_TURING_VAPSS_GDS_HW_CTRL_ADDR,v)
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAPSS_GDS_HW_CTRL_ADDR,m,v,HWIO_TURING_VAPSS_GDS_HW_CTRL_IN)
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_POWER_ON_STATUS_BMSK                                                        0x80000000
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_POWER_ON_STATUS_SHFT                                                              0x1f
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_HALT1_PWR_DOWN_ACK_STATUS_BMSK                                              0x78000000
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_HALT1_PWR_DOWN_ACK_STATUS_SHFT                                                    0x1b
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_HALT1_PWR_UP_ACK_STATUS_BMSK                                                 0x7800000
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_HALT1_PWR_UP_ACK_STATUS_SHFT                                                      0x17
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_HALT2_PWR_DOWN_ACK_STATUS_BMSK                                                0x780000
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_HALT2_PWR_DOWN_ACK_STATUS_SHFT                                                    0x13
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_HALT2_PWR_UP_ACK_STATUS_BMSK                                                   0x78000
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_HALT2_PWR_UP_ACK_STATUS_SHFT                                                       0xf
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_COLLAPSE_OUT_BMSK                                                               0x4000
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_COLLAPSE_OUT_SHFT                                                                  0xe
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_RESERVE_BITS13_BMSK                                                             0x2000
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_RESERVE_BITS13_SHFT                                                                0xd
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_HALT_ACK_TIME_OUT_BMSK                                                          0x1fe0
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_HALT_ACK_TIME_OUT_SHFT                                                             0x5
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_GDS_HW_STATE_BMSK                                                                 0x1e
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_GDS_HW_STATE_SHFT                                                                  0x1
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_SW_OVERRIDE_BMSK                                                                   0x1
#define HWIO_TURING_VAPSS_GDS_HW_CTRL_SW_OVERRIDE_SHFT                                                                   0x0

#define HWIO_TURING_VAPSS_BCR_ADDR                                                                                (TURING_CC_REG_BASE      + 0x00017000)
#define HWIO_TURING_VAPSS_BCR_RMSK                                                                                0x80000003
#define HWIO_TURING_VAPSS_BCR_IN          \
        in_dword_masked(HWIO_TURING_VAPSS_BCR_ADDR, HWIO_TURING_VAPSS_BCR_RMSK)
#define HWIO_TURING_VAPSS_BCR_INM(m)      \
        in_dword_masked(HWIO_TURING_VAPSS_BCR_ADDR, m)
#define HWIO_TURING_VAPSS_BCR_OUT(v)      \
        out_dword(HWIO_TURING_VAPSS_BCR_ADDR,v)
#define HWIO_TURING_VAPSS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAPSS_BCR_ADDR,m,v,HWIO_TURING_VAPSS_BCR_IN)
#define HWIO_TURING_VAPSS_BCR_DFD_STATUS_BMSK                                                                     0x80000000
#define HWIO_TURING_VAPSS_BCR_DFD_STATUS_SHFT                                                                           0x1f
#define HWIO_TURING_VAPSS_BCR_DFD_EN_BMSK                                                                                0x2
#define HWIO_TURING_VAPSS_BCR_DFD_EN_SHFT                                                                                0x1
#define HWIO_TURING_VAPSS_BCR_BLK_ARES_BMSK                                                                              0x1
#define HWIO_TURING_VAPSS_BCR_BLK_ARES_SHFT                                                                              0x0

#define HWIO_TURING_VAPSS_BCR_SLP_CBCR_ADDR                                                                       (TURING_CC_REG_BASE      + 0x00017004)
#define HWIO_TURING_VAPSS_BCR_SLP_CBCR_RMSK                                                                       0x80000003
#define HWIO_TURING_VAPSS_BCR_SLP_CBCR_IN          \
        in_dword_masked(HWIO_TURING_VAPSS_BCR_SLP_CBCR_ADDR, HWIO_TURING_VAPSS_BCR_SLP_CBCR_RMSK)
#define HWIO_TURING_VAPSS_BCR_SLP_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_VAPSS_BCR_SLP_CBCR_ADDR, m)
#define HWIO_TURING_VAPSS_BCR_SLP_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_VAPSS_BCR_SLP_CBCR_ADDR,v)
#define HWIO_TURING_VAPSS_BCR_SLP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAPSS_BCR_SLP_CBCR_ADDR,m,v,HWIO_TURING_VAPSS_BCR_SLP_CBCR_IN)
#define HWIO_TURING_VAPSS_BCR_SLP_CBCR_CLK_OFF_BMSK                                                               0x80000000
#define HWIO_TURING_VAPSS_BCR_SLP_CBCR_CLK_OFF_SHFT                                                                     0x1f
#define HWIO_TURING_VAPSS_BCR_SLP_CBCR_HW_CTL_BMSK                                                                       0x2
#define HWIO_TURING_VAPSS_BCR_SLP_CBCR_HW_CTL_SHFT                                                                       0x1
#define HWIO_TURING_VAPSS_BCR_SLP_CBCR_CLK_ENABLE_BMSK                                                                   0x1
#define HWIO_TURING_VAPSS_BCR_SLP_CBCR_CLK_ENABLE_SHFT                                                                   0x0

#define HWIO_TURING_VAPSS_AXI_CBCR_ADDR                                                                           (TURING_CC_REG_BASE      + 0x00017008)
#define HWIO_TURING_VAPSS_AXI_CBCR_RMSK                                                                           0x80007ff3
#define HWIO_TURING_VAPSS_AXI_CBCR_IN          \
        in_dword_masked(HWIO_TURING_VAPSS_AXI_CBCR_ADDR, HWIO_TURING_VAPSS_AXI_CBCR_RMSK)
#define HWIO_TURING_VAPSS_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_VAPSS_AXI_CBCR_ADDR, m)
#define HWIO_TURING_VAPSS_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_VAPSS_AXI_CBCR_ADDR,v)
#define HWIO_TURING_VAPSS_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAPSS_AXI_CBCR_ADDR,m,v,HWIO_TURING_VAPSS_AXI_CBCR_IN)
#define HWIO_TURING_VAPSS_AXI_CBCR_CLK_OFF_BMSK                                                                   0x80000000
#define HWIO_TURING_VAPSS_AXI_CBCR_CLK_OFF_SHFT                                                                         0x1f
#define HWIO_TURING_VAPSS_AXI_CBCR_FORCE_MEM_CORE_ON_BMSK                                                             0x4000
#define HWIO_TURING_VAPSS_AXI_CBCR_FORCE_MEM_CORE_ON_SHFT                                                                0xe
#define HWIO_TURING_VAPSS_AXI_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                                           0x2000
#define HWIO_TURING_VAPSS_AXI_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                              0xd
#define HWIO_TURING_VAPSS_AXI_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                                          0x1000
#define HWIO_TURING_VAPSS_AXI_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                             0xc
#define HWIO_TURING_VAPSS_AXI_CBCR_WAKEUP_BMSK                                                                         0xf00
#define HWIO_TURING_VAPSS_AXI_CBCR_WAKEUP_SHFT                                                                           0x8
#define HWIO_TURING_VAPSS_AXI_CBCR_SLEEP_BMSK                                                                           0xf0
#define HWIO_TURING_VAPSS_AXI_CBCR_SLEEP_SHFT                                                                            0x4
#define HWIO_TURING_VAPSS_AXI_CBCR_HW_CTL_BMSK                                                                           0x2
#define HWIO_TURING_VAPSS_AXI_CBCR_HW_CTL_SHFT                                                                           0x1
#define HWIO_TURING_VAPSS_AXI_CBCR_CLK_ENABLE_BMSK                                                                       0x1
#define HWIO_TURING_VAPSS_AXI_CBCR_CLK_ENABLE_SHFT                                                                       0x0

#define HWIO_TURING_VAPSS_AHBS_AON_CBCR_ADDR                                                                      (TURING_CC_REG_BASE      + 0x0001700c)
#define HWIO_TURING_VAPSS_AHBS_AON_CBCR_RMSK                                                                      0x80000003
#define HWIO_TURING_VAPSS_AHBS_AON_CBCR_IN          \
        in_dword_masked(HWIO_TURING_VAPSS_AHBS_AON_CBCR_ADDR, HWIO_TURING_VAPSS_AHBS_AON_CBCR_RMSK)
#define HWIO_TURING_VAPSS_AHBS_AON_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_VAPSS_AHBS_AON_CBCR_ADDR, m)
#define HWIO_TURING_VAPSS_AHBS_AON_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_VAPSS_AHBS_AON_CBCR_ADDR,v)
#define HWIO_TURING_VAPSS_AHBS_AON_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAPSS_AHBS_AON_CBCR_ADDR,m,v,HWIO_TURING_VAPSS_AHBS_AON_CBCR_IN)
#define HWIO_TURING_VAPSS_AHBS_AON_CBCR_CLK_OFF_BMSK                                                              0x80000000
#define HWIO_TURING_VAPSS_AHBS_AON_CBCR_CLK_OFF_SHFT                                                                    0x1f
#define HWIO_TURING_VAPSS_AHBS_AON_CBCR_HW_CTL_BMSK                                                                      0x2
#define HWIO_TURING_VAPSS_AHBS_AON_CBCR_HW_CTL_SHFT                                                                      0x1
#define HWIO_TURING_VAPSS_AHBS_AON_CBCR_CLK_ENABLE_BMSK                                                                  0x1
#define HWIO_TURING_VAPSS_AHBS_AON_CBCR_CLK_ENABLE_SHFT                                                                  0x0

#define HWIO_TURING_VAP_CORE_CMD_RCGR_ADDR                                                                        (TURING_CC_REG_BASE      + 0x00018000)
#define HWIO_TURING_VAP_CORE_CMD_RCGR_RMSK                                                                        0x800000f3
#define HWIO_TURING_VAP_CORE_CMD_RCGR_IN          \
        in_dword_masked(HWIO_TURING_VAP_CORE_CMD_RCGR_ADDR, HWIO_TURING_VAP_CORE_CMD_RCGR_RMSK)
#define HWIO_TURING_VAP_CORE_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_TURING_VAP_CORE_CMD_RCGR_ADDR, m)
#define HWIO_TURING_VAP_CORE_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_TURING_VAP_CORE_CMD_RCGR_ADDR,v)
#define HWIO_TURING_VAP_CORE_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAP_CORE_CMD_RCGR_ADDR,m,v,HWIO_TURING_VAP_CORE_CMD_RCGR_IN)
#define HWIO_TURING_VAP_CORE_CMD_RCGR_ROOT_OFF_BMSK                                                               0x80000000
#define HWIO_TURING_VAP_CORE_CMD_RCGR_ROOT_OFF_SHFT                                                                     0x1f
#define HWIO_TURING_VAP_CORE_CMD_RCGR_DIRTY_D_BMSK                                                                      0x80
#define HWIO_TURING_VAP_CORE_CMD_RCGR_DIRTY_D_SHFT                                                                       0x7
#define HWIO_TURING_VAP_CORE_CMD_RCGR_DIRTY_N_BMSK                                                                      0x40
#define HWIO_TURING_VAP_CORE_CMD_RCGR_DIRTY_N_SHFT                                                                       0x6
#define HWIO_TURING_VAP_CORE_CMD_RCGR_DIRTY_M_BMSK                                                                      0x20
#define HWIO_TURING_VAP_CORE_CMD_RCGR_DIRTY_M_SHFT                                                                       0x5
#define HWIO_TURING_VAP_CORE_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                               0x10
#define HWIO_TURING_VAP_CORE_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                                0x4
#define HWIO_TURING_VAP_CORE_CMD_RCGR_ROOT_EN_BMSK                                                                       0x2
#define HWIO_TURING_VAP_CORE_CMD_RCGR_ROOT_EN_SHFT                                                                       0x1
#define HWIO_TURING_VAP_CORE_CMD_RCGR_UPDATE_BMSK                                                                        0x1
#define HWIO_TURING_VAP_CORE_CMD_RCGR_UPDATE_SHFT                                                                        0x0

#define HWIO_TURING_VAP_CORE_CFG_RCGR_ADDR                                                                        (TURING_CC_REG_BASE      + 0x00018004)
#define HWIO_TURING_VAP_CORE_CFG_RCGR_RMSK                                                                            0x771f
#define HWIO_TURING_VAP_CORE_CFG_RCGR_IN          \
        in_dword_masked(HWIO_TURING_VAP_CORE_CFG_RCGR_ADDR, HWIO_TURING_VAP_CORE_CFG_RCGR_RMSK)
#define HWIO_TURING_VAP_CORE_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_TURING_VAP_CORE_CFG_RCGR_ADDR, m)
#define HWIO_TURING_VAP_CORE_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_TURING_VAP_CORE_CFG_RCGR_ADDR,v)
#define HWIO_TURING_VAP_CORE_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAP_CORE_CFG_RCGR_ADDR,m,v,HWIO_TURING_VAP_CORE_CFG_RCGR_IN)
#define HWIO_TURING_VAP_CORE_CFG_RCGR_ALT_SRC_SEL_BMSK                                                                0x4000
#define HWIO_TURING_VAP_CORE_CFG_RCGR_ALT_SRC_SEL_SHFT                                                                   0xe
#define HWIO_TURING_VAP_CORE_CFG_RCGR_MODE_BMSK                                                                       0x3000
#define HWIO_TURING_VAP_CORE_CFG_RCGR_MODE_SHFT                                                                          0xc
#define HWIO_TURING_VAP_CORE_CFG_RCGR_SRC_SEL_BMSK                                                                     0x700
#define HWIO_TURING_VAP_CORE_CFG_RCGR_SRC_SEL_SHFT                                                                       0x8
#define HWIO_TURING_VAP_CORE_CFG_RCGR_SRC_DIV_BMSK                                                                      0x1f
#define HWIO_TURING_VAP_CORE_CFG_RCGR_SRC_DIV_SHFT                                                                       0x0

#define HWIO_TURING_VAP_CORE_M_ADDR                                                                               (TURING_CC_REG_BASE      + 0x00018008)
#define HWIO_TURING_VAP_CORE_M_RMSK                                                                                     0xff
#define HWIO_TURING_VAP_CORE_M_IN          \
        in_dword_masked(HWIO_TURING_VAP_CORE_M_ADDR, HWIO_TURING_VAP_CORE_M_RMSK)
#define HWIO_TURING_VAP_CORE_M_INM(m)      \
        in_dword_masked(HWIO_TURING_VAP_CORE_M_ADDR, m)
#define HWIO_TURING_VAP_CORE_M_OUT(v)      \
        out_dword(HWIO_TURING_VAP_CORE_M_ADDR,v)
#define HWIO_TURING_VAP_CORE_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAP_CORE_M_ADDR,m,v,HWIO_TURING_VAP_CORE_M_IN)
#define HWIO_TURING_VAP_CORE_M_M_BMSK                                                                                   0xff
#define HWIO_TURING_VAP_CORE_M_M_SHFT                                                                                    0x0

#define HWIO_TURING_VAP_CORE_N_ADDR                                                                               (TURING_CC_REG_BASE      + 0x0001800c)
#define HWIO_TURING_VAP_CORE_N_RMSK                                                                                     0xff
#define HWIO_TURING_VAP_CORE_N_IN          \
        in_dword_masked(HWIO_TURING_VAP_CORE_N_ADDR, HWIO_TURING_VAP_CORE_N_RMSK)
#define HWIO_TURING_VAP_CORE_N_INM(m)      \
        in_dword_masked(HWIO_TURING_VAP_CORE_N_ADDR, m)
#define HWIO_TURING_VAP_CORE_N_OUT(v)      \
        out_dword(HWIO_TURING_VAP_CORE_N_ADDR,v)
#define HWIO_TURING_VAP_CORE_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAP_CORE_N_ADDR,m,v,HWIO_TURING_VAP_CORE_N_IN)
#define HWIO_TURING_VAP_CORE_N_NOT_N_MINUS_M_BMSK                                                                       0xff
#define HWIO_TURING_VAP_CORE_N_NOT_N_MINUS_M_SHFT                                                                        0x0

#define HWIO_TURING_VAP_CORE_D_ADDR                                                                               (TURING_CC_REG_BASE      + 0x00018010)
#define HWIO_TURING_VAP_CORE_D_RMSK                                                                                     0xff
#define HWIO_TURING_VAP_CORE_D_IN          \
        in_dword_masked(HWIO_TURING_VAP_CORE_D_ADDR, HWIO_TURING_VAP_CORE_D_RMSK)
#define HWIO_TURING_VAP_CORE_D_INM(m)      \
        in_dword_masked(HWIO_TURING_VAP_CORE_D_ADDR, m)
#define HWIO_TURING_VAP_CORE_D_OUT(v)      \
        out_dword(HWIO_TURING_VAP_CORE_D_ADDR,v)
#define HWIO_TURING_VAP_CORE_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAP_CORE_D_ADDR,m,v,HWIO_TURING_VAP_CORE_D_IN)
#define HWIO_TURING_VAP_CORE_D_NOT_2D_BMSK                                                                              0xff
#define HWIO_TURING_VAP_CORE_D_NOT_2D_SHFT                                                                               0x0

#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_ADDR                                                                      (TURING_CC_REG_BASE      + 0x00018014)
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_RMSK                                                                      0x80007ff3
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_IN          \
        in_dword_masked(HWIO_TURING_VAPSS_VAP_CORE_CBCR_ADDR, HWIO_TURING_VAPSS_VAP_CORE_CBCR_RMSK)
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_VAPSS_VAP_CORE_CBCR_ADDR, m)
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_VAPSS_VAP_CORE_CBCR_ADDR,v)
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAPSS_VAP_CORE_CBCR_ADDR,m,v,HWIO_TURING_VAPSS_VAP_CORE_CBCR_IN)
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_CLK_OFF_BMSK                                                              0x80000000
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_CLK_OFF_SHFT                                                                    0x1f
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                                        0x4000
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                                           0xe
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                                      0x2000
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                         0xd
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                                     0x1000
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                        0xc
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_WAKEUP_BMSK                                                                    0xf00
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_WAKEUP_SHFT                                                                      0x8
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_SLEEP_BMSK                                                                      0xf0
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_SLEEP_SHFT                                                                       0x4
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_HW_CTL_BMSK                                                                      0x2
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_HW_CTL_SHFT                                                                      0x1
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_CLK_ENABLE_BMSK                                                                  0x1
#define HWIO_TURING_VAPSS_VAP_CORE_CBCR_CLK_ENABLE_SHFT                                                                  0x0

#define HWIO_TURING_VAP_TCMS_CMD_RCGR_ADDR                                                                        (TURING_CC_REG_BASE      + 0x00019000)
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_RMSK                                                                        0x800000f3
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_TURING_VAP_TCMS_CMD_RCGR_ADDR, HWIO_TURING_VAP_TCMS_CMD_RCGR_RMSK)
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_TURING_VAP_TCMS_CMD_RCGR_ADDR, m)
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_TURING_VAP_TCMS_CMD_RCGR_ADDR,v)
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAP_TCMS_CMD_RCGR_ADDR,m,v,HWIO_TURING_VAP_TCMS_CMD_RCGR_IN)
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_ROOT_OFF_BMSK                                                               0x80000000
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_ROOT_OFF_SHFT                                                                     0x1f
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_DIRTY_D_BMSK                                                                      0x80
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_DIRTY_D_SHFT                                                                       0x7
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_DIRTY_N_BMSK                                                                      0x40
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_DIRTY_N_SHFT                                                                       0x6
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_DIRTY_M_BMSK                                                                      0x20
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_DIRTY_M_SHFT                                                                       0x5
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                               0x10
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                                0x4
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_ROOT_EN_BMSK                                                                       0x2
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_ROOT_EN_SHFT                                                                       0x1
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_UPDATE_BMSK                                                                        0x1
#define HWIO_TURING_VAP_TCMS_CMD_RCGR_UPDATE_SHFT                                                                        0x0

#define HWIO_TURING_VAP_TCMS_CFG_RCGR_ADDR                                                                        (TURING_CC_REG_BASE      + 0x00019004)
#define HWIO_TURING_VAP_TCMS_CFG_RCGR_RMSK                                                                            0x771f
#define HWIO_TURING_VAP_TCMS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_TURING_VAP_TCMS_CFG_RCGR_ADDR, HWIO_TURING_VAP_TCMS_CFG_RCGR_RMSK)
#define HWIO_TURING_VAP_TCMS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_TURING_VAP_TCMS_CFG_RCGR_ADDR, m)
#define HWIO_TURING_VAP_TCMS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_TURING_VAP_TCMS_CFG_RCGR_ADDR,v)
#define HWIO_TURING_VAP_TCMS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAP_TCMS_CFG_RCGR_ADDR,m,v,HWIO_TURING_VAP_TCMS_CFG_RCGR_IN)
#define HWIO_TURING_VAP_TCMS_CFG_RCGR_ALT_SRC_SEL_BMSK                                                                0x4000
#define HWIO_TURING_VAP_TCMS_CFG_RCGR_ALT_SRC_SEL_SHFT                                                                   0xe
#define HWIO_TURING_VAP_TCMS_CFG_RCGR_MODE_BMSK                                                                       0x3000
#define HWIO_TURING_VAP_TCMS_CFG_RCGR_MODE_SHFT                                                                          0xc
#define HWIO_TURING_VAP_TCMS_CFG_RCGR_SRC_SEL_BMSK                                                                     0x700
#define HWIO_TURING_VAP_TCMS_CFG_RCGR_SRC_SEL_SHFT                                                                       0x8
#define HWIO_TURING_VAP_TCMS_CFG_RCGR_SRC_DIV_BMSK                                                                      0x1f
#define HWIO_TURING_VAP_TCMS_CFG_RCGR_SRC_DIV_SHFT                                                                       0x0

#define HWIO_TURING_VAP_TCMS_M_ADDR                                                                               (TURING_CC_REG_BASE      + 0x00019008)
#define HWIO_TURING_VAP_TCMS_M_RMSK                                                                                     0xff
#define HWIO_TURING_VAP_TCMS_M_IN          \
        in_dword_masked(HWIO_TURING_VAP_TCMS_M_ADDR, HWIO_TURING_VAP_TCMS_M_RMSK)
#define HWIO_TURING_VAP_TCMS_M_INM(m)      \
        in_dword_masked(HWIO_TURING_VAP_TCMS_M_ADDR, m)
#define HWIO_TURING_VAP_TCMS_M_OUT(v)      \
        out_dword(HWIO_TURING_VAP_TCMS_M_ADDR,v)
#define HWIO_TURING_VAP_TCMS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAP_TCMS_M_ADDR,m,v,HWIO_TURING_VAP_TCMS_M_IN)
#define HWIO_TURING_VAP_TCMS_M_M_BMSK                                                                                   0xff
#define HWIO_TURING_VAP_TCMS_M_M_SHFT                                                                                    0x0

#define HWIO_TURING_VAP_TCMS_N_ADDR                                                                               (TURING_CC_REG_BASE      + 0x0001900c)
#define HWIO_TURING_VAP_TCMS_N_RMSK                                                                                     0xff
#define HWIO_TURING_VAP_TCMS_N_IN          \
        in_dword_masked(HWIO_TURING_VAP_TCMS_N_ADDR, HWIO_TURING_VAP_TCMS_N_RMSK)
#define HWIO_TURING_VAP_TCMS_N_INM(m)      \
        in_dword_masked(HWIO_TURING_VAP_TCMS_N_ADDR, m)
#define HWIO_TURING_VAP_TCMS_N_OUT(v)      \
        out_dword(HWIO_TURING_VAP_TCMS_N_ADDR,v)
#define HWIO_TURING_VAP_TCMS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAP_TCMS_N_ADDR,m,v,HWIO_TURING_VAP_TCMS_N_IN)
#define HWIO_TURING_VAP_TCMS_N_NOT_N_MINUS_M_BMSK                                                                       0xff
#define HWIO_TURING_VAP_TCMS_N_NOT_N_MINUS_M_SHFT                                                                        0x0

#define HWIO_TURING_VAP_TCMS_D_ADDR                                                                               (TURING_CC_REG_BASE      + 0x00019010)
#define HWIO_TURING_VAP_TCMS_D_RMSK                                                                                     0xff
#define HWIO_TURING_VAP_TCMS_D_IN          \
        in_dword_masked(HWIO_TURING_VAP_TCMS_D_ADDR, HWIO_TURING_VAP_TCMS_D_RMSK)
#define HWIO_TURING_VAP_TCMS_D_INM(m)      \
        in_dword_masked(HWIO_TURING_VAP_TCMS_D_ADDR, m)
#define HWIO_TURING_VAP_TCMS_D_OUT(v)      \
        out_dword(HWIO_TURING_VAP_TCMS_D_ADDR,v)
#define HWIO_TURING_VAP_TCMS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAP_TCMS_D_ADDR,m,v,HWIO_TURING_VAP_TCMS_D_IN)
#define HWIO_TURING_VAP_TCMS_D_NOT_2D_BMSK                                                                              0xff
#define HWIO_TURING_VAP_TCMS_D_NOT_2D_SHFT                                                                               0x0

#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_ADDR                                                                      (TURING_CC_REG_BASE      + 0x00019014)
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_RMSK                                                                      0x80007ff3
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_IN          \
        in_dword_masked(HWIO_TURING_VAPSS_VAP_TCMS_CBCR_ADDR, HWIO_TURING_VAPSS_VAP_TCMS_CBCR_RMSK)
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_VAPSS_VAP_TCMS_CBCR_ADDR, m)
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_VAPSS_VAP_TCMS_CBCR_ADDR,v)
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAPSS_VAP_TCMS_CBCR_ADDR,m,v,HWIO_TURING_VAPSS_VAP_TCMS_CBCR_IN)
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_CLK_OFF_BMSK                                                              0x80000000
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_CLK_OFF_SHFT                                                                    0x1f
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_FORCE_MEM_CORE_ON_BMSK                                                        0x4000
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_FORCE_MEM_CORE_ON_SHFT                                                           0xe
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                                      0x2000
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                         0xd
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                                     0x1000
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                        0xc
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_WAKEUP_BMSK                                                                    0xf00
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_WAKEUP_SHFT                                                                      0x8
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_SLEEP_BMSK                                                                      0xf0
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_SLEEP_SHFT                                                                       0x4
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_HW_CTL_BMSK                                                                      0x2
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_HW_CTL_SHFT                                                                      0x1
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_CLK_ENABLE_BMSK                                                                  0x1
#define HWIO_TURING_VAPSS_VAP_TCMS_CBCR_CLK_ENABLE_SHFT                                                                  0x0

#define HWIO_TURING_VAPSS_XO_CBCR_ADDR                                                                            (TURING_CC_REG_BASE      + 0x00019018)
#define HWIO_TURING_VAPSS_XO_CBCR_RMSK                                                                            0x80000003
#define HWIO_TURING_VAPSS_XO_CBCR_IN          \
        in_dword_masked(HWIO_TURING_VAPSS_XO_CBCR_ADDR, HWIO_TURING_VAPSS_XO_CBCR_RMSK)
#define HWIO_TURING_VAPSS_XO_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_VAPSS_XO_CBCR_ADDR, m)
#define HWIO_TURING_VAPSS_XO_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_VAPSS_XO_CBCR_ADDR,v)
#define HWIO_TURING_VAPSS_XO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_VAPSS_XO_CBCR_ADDR,m,v,HWIO_TURING_VAPSS_XO_CBCR_IN)
#define HWIO_TURING_VAPSS_XO_CBCR_CLK_OFF_BMSK                                                                    0x80000000
#define HWIO_TURING_VAPSS_XO_CBCR_CLK_OFF_SHFT                                                                          0x1f
#define HWIO_TURING_VAPSS_XO_CBCR_HW_CTL_BMSK                                                                            0x2
#define HWIO_TURING_VAPSS_XO_CBCR_HW_CTL_SHFT                                                                            0x1
#define HWIO_TURING_VAPSS_XO_CBCR_CLK_ENABLE_BMSK                                                                        0x1
#define HWIO_TURING_VAPSS_XO_CBCR_CLK_ENABLE_SHFT                                                                        0x0

#define HWIO_TURING_Q6SS_AXIS2_CBCR_ADDR                                                                          (TURING_CC_REG_BASE      + 0x0001a000)
#define HWIO_TURING_Q6SS_AXIS2_CBCR_RMSK                                                                          0x80000003
#define HWIO_TURING_Q6SS_AXIS2_CBCR_IN          \
        in_dword_masked(HWIO_TURING_Q6SS_AXIS2_CBCR_ADDR, HWIO_TURING_Q6SS_AXIS2_CBCR_RMSK)
#define HWIO_TURING_Q6SS_AXIS2_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_Q6SS_AXIS2_CBCR_ADDR, m)
#define HWIO_TURING_Q6SS_AXIS2_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_Q6SS_AXIS2_CBCR_ADDR,v)
#define HWIO_TURING_Q6SS_AXIS2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_Q6SS_AXIS2_CBCR_ADDR,m,v,HWIO_TURING_Q6SS_AXIS2_CBCR_IN)
#define HWIO_TURING_Q6SS_AXIS2_CBCR_CLK_OFF_BMSK                                                                  0x80000000
#define HWIO_TURING_Q6SS_AXIS2_CBCR_CLK_OFF_SHFT                                                                        0x1f
#define HWIO_TURING_Q6SS_AXIS2_CBCR_HW_CTL_BMSK                                                                          0x2
#define HWIO_TURING_Q6SS_AXIS2_CBCR_HW_CTL_SHFT                                                                          0x1
#define HWIO_TURING_Q6SS_AXIS2_CBCR_CLK_ENABLE_BMSK                                                                      0x1
#define HWIO_TURING_Q6SS_AXIS2_CBCR_CLK_ENABLE_SHFT                                                                      0x0

#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_ADDR                                                         (TURING_CC_REG_BASE      + 0x0001b000)
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_RMSK                                                         0x80007ff3
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_ADDR, HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_RMSK)
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_IN)
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_FORCE_MEM_CORE_ON_BMSK                                           0x4000
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_FORCE_MEM_CORE_ON_SHFT                                              0xe
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                         0x2000
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                            0xd
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                        0x1000
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                           0xc
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_WAKEUP_BMSK                                                       0xf00
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_WAKEUP_SHFT                                                         0x8
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_SLEEP_BMSK                                                         0xf0
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_SLEEP_SHFT                                                          0x4
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_HW_CTL_BMSK                                                         0x2
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_HW_CTL_SHFT                                                         0x1
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AXI_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_ADDR                                                         (TURING_CC_REG_BASE      + 0x0001c000)
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_RMSK                                                         0x80000003
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_ADDR, HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_RMSK)
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_IN)
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_HW_CTL_BMSK                                                         0x2
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_HW_CTL_SHFT                                                         0x1
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_ADDR                                                              (TURING_CC_REG_BASE      + 0x00021000)
#define HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_RMSK                                                              0x80000003
#define HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_ADDR, HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_RMSK)
#define HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_IN)
#define HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_CLK_OFF_BMSK                                                      0x80000000
#define HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_CLK_OFF_SHFT                                                            0x1f
#define HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_HW_CTL_BMSK                                                              0x2
#define HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_HW_CTL_SHFT                                                              0x1
#define HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_CLK_ENABLE_BMSK                                                          0x1
#define HWIO_TURING_TURING_WRAPPER_RSCC_XO_CBCR_CLK_ENABLE_SHFT                                                          0x0

#define HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_ADDR                                                             (TURING_CC_REG_BASE      + 0x00021004)
#define HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_RMSK                                                             0x80000003
#define HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_ADDR, HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_RMSK)
#define HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_IN)
#define HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_HW_CTL_BMSK                                                             0x2
#define HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_HW_CTL_SHFT                                                             0x1
#define HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_TURING_TURING_WRAPPER_RSCC_AON_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_ADDR                                                             (TURING_CC_REG_BASE      + 0x00021008)
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_RMSK                                                                    0xf
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_ADDR, HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_RMSK)
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_IN)
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_RSC_BR_EVENT_BMSK                                                       0xf
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_RSC_BR_EVENT_SHFT                                                       0x0

#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_MASK_ADDR                                               (TURING_CC_REG_BASE      + 0x0002100c)
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_MASK_RMSK                                                      0xf
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_MASK_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_MASK_ADDR, HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_MASK_RMSK)
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_MASK_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_MASK_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_MASK_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_MASK_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_MASK_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_MASK_IN)
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_MASK_RSC_BR_EVENT_OVERRIDE_MASK_BMSK                           0xf
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_MASK_RSC_BR_EVENT_OVERRIDE_MASK_SHFT                           0x0

#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_VAL_ADDR                                                (TURING_CC_REG_BASE      + 0x00021010)
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_VAL_RMSK                                                       0xf
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_VAL_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_VAL_ADDR, HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_VAL_RMSK)
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_VAL_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_VAL_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_VAL_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_VAL_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_VAL_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_VAL_IN)
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_VAL_RSC_BR_EVENT_OVERRIDE_VAL_BMSK                             0xf
#define HWIO_TURING_TURING_WRAPPER_RSCC_BR_EVENT_OVERRIDE_VAL_RSC_BR_EVENT_OVERRIDE_VAL_SHFT                             0x0

#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_MASK_ADDR                                               (TURING_CC_REG_BASE      + 0x00021014)
#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_MASK_RMSK                                               0xfffffc00
#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_MASK_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_MASK_ADDR, HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_MASK_RMSK)
#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_MASK_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_MASK_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_MASK_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_MASK_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_MASK_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_MASK_IN)
#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_MASK_RSC_PWR_CTRL_OVERRIDE_MASK_BMSK                    0xfffffc00
#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_MASK_RSC_PWR_CTRL_OVERRIDE_MASK_SHFT                           0xa

#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_VAL_ADDR                                                (TURING_CC_REG_BASE      + 0x00021018)
#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_VAL_RMSK                                                0xfffffc00
#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_VAL_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_VAL_ADDR, HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_VAL_RMSK)
#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_VAL_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_VAL_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_VAL_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_VAL_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_VAL_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_VAL_IN)
#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_VAL_RSC_PWR_CTRL_OVERRIDE_VAL_BMSK                      0xfffffc00
#define HWIO_TURING_TURING_WRAPPER_RSCC_PWR_CTRL_OVERRIDE_VAL_RSC_PWR_CTRL_OVERRIDE_VAL_SHFT                             0xa

#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_MASK_ADDR                                             (TURING_CC_REG_BASE      + 0x0002101c)
#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_MASK_RMSK                                             0xfffffc00
#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_MASK_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_MASK_ADDR, HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_MASK_RMSK)
#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_MASK_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_MASK_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_MASK_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_MASK_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_MASK_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_MASK_IN)
#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_MASK_RSC_WAIT_EVENT_OVERRIDE_MASK_BMSK                0xfffffc00
#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_MASK_RSC_WAIT_EVENT_OVERRIDE_MASK_SHFT                       0xa

#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_VAL_ADDR                                              (TURING_CC_REG_BASE      + 0x00021020)
#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_VAL_RMSK                                              0xfffffc00
#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_VAL_IN          \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_VAL_ADDR, HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_VAL_RMSK)
#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_VAL_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_VAL_ADDR, m)
#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_VAL_OUT(v)      \
        out_dword(HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_VAL_ADDR,v)
#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_VAL_ADDR,m,v,HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_VAL_IN)
#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_VAL_RSC_WAIT_EVENT_OVERRIDE_VAL_BMSK                  0xfffffc00
#define HWIO_TURING_TURING_WRAPPER_RSCC_WAIT_EVENT_OVERRIDE_VAL_RSC_WAIT_EVENT_OVERRIDE_VAL_SHFT                         0xa

#define HWIO_TURING_TURING_CC_DEBUG_MUX_MUXR_ADDR                                                                 (TURING_CC_REG_BASE      + 0x00022000)
#define HWIO_TURING_TURING_CC_DEBUG_MUX_MUXR_RMSK                                                                     0xffff
#define HWIO_TURING_TURING_CC_DEBUG_MUX_MUXR_IN          \
        in_dword_masked(HWIO_TURING_TURING_CC_DEBUG_MUX_MUXR_ADDR, HWIO_TURING_TURING_CC_DEBUG_MUX_MUXR_RMSK)
#define HWIO_TURING_TURING_CC_DEBUG_MUX_MUXR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_CC_DEBUG_MUX_MUXR_ADDR, m)
#define HWIO_TURING_TURING_CC_DEBUG_MUX_MUXR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_CC_DEBUG_MUX_MUXR_ADDR,v)
#define HWIO_TURING_TURING_CC_DEBUG_MUX_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_CC_DEBUG_MUX_MUXR_ADDR,m,v,HWIO_TURING_TURING_CC_DEBUG_MUX_MUXR_IN)
#define HWIO_TURING_TURING_CC_DEBUG_MUX_MUXR_MUX_SEL_BMSK                                                             0xffff
#define HWIO_TURING_TURING_CC_DEBUG_MUX_MUXR_MUX_SEL_SHFT                                                                0x0

#define HWIO_TURING_TURING_CC_DEBUG_DIV_CDIVR_ADDR                                                                (TURING_CC_REG_BASE      + 0x00022004)
#define HWIO_TURING_TURING_CC_DEBUG_DIV_CDIVR_RMSK                                                                       0x3
#define HWIO_TURING_TURING_CC_DEBUG_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_TURING_TURING_CC_DEBUG_DIV_CDIVR_ADDR, HWIO_TURING_TURING_CC_DEBUG_DIV_CDIVR_RMSK)
#define HWIO_TURING_TURING_CC_DEBUG_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_CC_DEBUG_DIV_CDIVR_ADDR, m)
#define HWIO_TURING_TURING_CC_DEBUG_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_CC_DEBUG_DIV_CDIVR_ADDR,v)
#define HWIO_TURING_TURING_CC_DEBUG_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_CC_DEBUG_DIV_CDIVR_ADDR,m,v,HWIO_TURING_TURING_CC_DEBUG_DIV_CDIVR_IN)
#define HWIO_TURING_TURING_CC_DEBUG_DIV_CDIVR_CLK_DIV_BMSK                                                               0x3
#define HWIO_TURING_TURING_CC_DEBUG_DIV_CDIVR_CLK_DIV_SHFT                                                               0x0

#define HWIO_TURING_TURING_CC_DEBUG_CBCR_ADDR                                                                     (TURING_CC_REG_BASE      + 0x00022008)
#define HWIO_TURING_TURING_CC_DEBUG_CBCR_RMSK                                                                     0x80000001
#define HWIO_TURING_TURING_CC_DEBUG_CBCR_IN          \
        in_dword_masked(HWIO_TURING_TURING_CC_DEBUG_CBCR_ADDR, HWIO_TURING_TURING_CC_DEBUG_CBCR_RMSK)
#define HWIO_TURING_TURING_CC_DEBUG_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_CC_DEBUG_CBCR_ADDR, m)
#define HWIO_TURING_TURING_CC_DEBUG_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_CC_DEBUG_CBCR_ADDR,v)
#define HWIO_TURING_TURING_CC_DEBUG_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_CC_DEBUG_CBCR_ADDR,m,v,HWIO_TURING_TURING_CC_DEBUG_CBCR_IN)
#define HWIO_TURING_TURING_CC_DEBUG_CBCR_CLK_OFF_BMSK                                                             0x80000000
#define HWIO_TURING_TURING_CC_DEBUG_CBCR_CLK_OFF_SHFT                                                                   0x1f
#define HWIO_TURING_TURING_CC_DEBUG_CBCR_CLK_ENABLE_BMSK                                                                 0x1
#define HWIO_TURING_TURING_CC_DEBUG_CBCR_CLK_ENABLE_SHFT                                                                 0x0

#define HWIO_TURING_TURING_CC_PLL_TEST_MUX_MUXR_ADDR                                                              (TURING_CC_REG_BASE      + 0x0002200c)
#define HWIO_TURING_TURING_CC_PLL_TEST_MUX_MUXR_RMSK                                                                     0x3
#define HWIO_TURING_TURING_CC_PLL_TEST_MUX_MUXR_IN          \
        in_dword_masked(HWIO_TURING_TURING_CC_PLL_TEST_MUX_MUXR_ADDR, HWIO_TURING_TURING_CC_PLL_TEST_MUX_MUXR_RMSK)
#define HWIO_TURING_TURING_CC_PLL_TEST_MUX_MUXR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_CC_PLL_TEST_MUX_MUXR_ADDR, m)
#define HWIO_TURING_TURING_CC_PLL_TEST_MUX_MUXR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_CC_PLL_TEST_MUX_MUXR_ADDR,v)
#define HWIO_TURING_TURING_CC_PLL_TEST_MUX_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_CC_PLL_TEST_MUX_MUXR_ADDR,m,v,HWIO_TURING_TURING_CC_PLL_TEST_MUX_MUXR_IN)
#define HWIO_TURING_TURING_CC_PLL_TEST_MUX_MUXR_MUX_SEL_BMSK                                                             0x3
#define HWIO_TURING_TURING_CC_PLL_TEST_MUX_MUXR_MUX_SEL_SHFT                                                             0x0

#define HWIO_TURING_TURING_CC_PLL_TEST_DIV_CDIVR_ADDR                                                             (TURING_CC_REG_BASE      + 0x00022010)
#define HWIO_TURING_TURING_CC_PLL_TEST_DIV_CDIVR_RMSK                                                                    0x3
#define HWIO_TURING_TURING_CC_PLL_TEST_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_TURING_TURING_CC_PLL_TEST_DIV_CDIVR_ADDR, HWIO_TURING_TURING_CC_PLL_TEST_DIV_CDIVR_RMSK)
#define HWIO_TURING_TURING_CC_PLL_TEST_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_CC_PLL_TEST_DIV_CDIVR_ADDR, m)
#define HWIO_TURING_TURING_CC_PLL_TEST_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_CC_PLL_TEST_DIV_CDIVR_ADDR,v)
#define HWIO_TURING_TURING_CC_PLL_TEST_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_CC_PLL_TEST_DIV_CDIVR_ADDR,m,v,HWIO_TURING_TURING_CC_PLL_TEST_DIV_CDIVR_IN)
#define HWIO_TURING_TURING_CC_PLL_TEST_DIV_CDIVR_CLK_DIV_BMSK                                                            0x3
#define HWIO_TURING_TURING_CC_PLL_TEST_DIV_CDIVR_CLK_DIV_SHFT                                                            0x0

#define HWIO_TURING_TURING_CC_PLL_TEST_CBCR_ADDR                                                                  (TURING_CC_REG_BASE      + 0x00022014)
#define HWIO_TURING_TURING_CC_PLL_TEST_CBCR_RMSK                                                                  0x80000001
#define HWIO_TURING_TURING_CC_PLL_TEST_CBCR_IN          \
        in_dword_masked(HWIO_TURING_TURING_CC_PLL_TEST_CBCR_ADDR, HWIO_TURING_TURING_CC_PLL_TEST_CBCR_RMSK)
#define HWIO_TURING_TURING_CC_PLL_TEST_CBCR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_CC_PLL_TEST_CBCR_ADDR, m)
#define HWIO_TURING_TURING_CC_PLL_TEST_CBCR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_CC_PLL_TEST_CBCR_ADDR,v)
#define HWIO_TURING_TURING_CC_PLL_TEST_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_CC_PLL_TEST_CBCR_ADDR,m,v,HWIO_TURING_TURING_CC_PLL_TEST_CBCR_IN)
#define HWIO_TURING_TURING_CC_PLL_TEST_CBCR_CLK_OFF_BMSK                                                          0x80000000
#define HWIO_TURING_TURING_CC_PLL_TEST_CBCR_CLK_OFF_SHFT                                                                0x1f
#define HWIO_TURING_TURING_CC_PLL_TEST_CBCR_CLK_ENABLE_BMSK                                                              0x1
#define HWIO_TURING_TURING_CC_PLL_TEST_CBCR_CLK_ENABLE_SHFT                                                              0x0

#define HWIO_TURING_TURING_CC_PLL_RESET_N_MUXR_ADDR                                                               (TURING_CC_REG_BASE      + 0x00022018)
#define HWIO_TURING_TURING_CC_PLL_RESET_N_MUXR_RMSK                                                                      0x7
#define HWIO_TURING_TURING_CC_PLL_RESET_N_MUXR_IN          \
        in_dword_masked(HWIO_TURING_TURING_CC_PLL_RESET_N_MUXR_ADDR, HWIO_TURING_TURING_CC_PLL_RESET_N_MUXR_RMSK)
#define HWIO_TURING_TURING_CC_PLL_RESET_N_MUXR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_CC_PLL_RESET_N_MUXR_ADDR, m)
#define HWIO_TURING_TURING_CC_PLL_RESET_N_MUXR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_CC_PLL_RESET_N_MUXR_ADDR,v)
#define HWIO_TURING_TURING_CC_PLL_RESET_N_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_CC_PLL_RESET_N_MUXR_ADDR,m,v,HWIO_TURING_TURING_CC_PLL_RESET_N_MUXR_IN)
#define HWIO_TURING_TURING_CC_PLL_RESET_N_MUXR_MUX_SEL_BMSK                                                              0x7
#define HWIO_TURING_TURING_CC_PLL_RESET_N_MUXR_MUX_SEL_SHFT                                                              0x0

#define HWIO_TURING_TURING_CC_PLL_BYPASSNL_MUXR_ADDR                                                              (TURING_CC_REG_BASE      + 0x00022020)
#define HWIO_TURING_TURING_CC_PLL_BYPASSNL_MUXR_RMSK                                                                     0x7
#define HWIO_TURING_TURING_CC_PLL_BYPASSNL_MUXR_IN          \
        in_dword_masked(HWIO_TURING_TURING_CC_PLL_BYPASSNL_MUXR_ADDR, HWIO_TURING_TURING_CC_PLL_BYPASSNL_MUXR_RMSK)
#define HWIO_TURING_TURING_CC_PLL_BYPASSNL_MUXR_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_CC_PLL_BYPASSNL_MUXR_ADDR, m)
#define HWIO_TURING_TURING_CC_PLL_BYPASSNL_MUXR_OUT(v)      \
        out_dword(HWIO_TURING_TURING_CC_PLL_BYPASSNL_MUXR_ADDR,v)
#define HWIO_TURING_TURING_CC_PLL_BYPASSNL_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_CC_PLL_BYPASSNL_MUXR_ADDR,m,v,HWIO_TURING_TURING_CC_PLL_BYPASSNL_MUXR_IN)
#define HWIO_TURING_TURING_CC_PLL_BYPASSNL_MUXR_MUX_SEL_BMSK                                                             0x7
#define HWIO_TURING_TURING_CC_PLL_BYPASSNL_MUXR_MUX_SEL_SHFT                                                             0x0

#define HWIO_TURING_TEST_BUS_SEL_ADDR                                                                             (TURING_CC_REG_BASE      + 0x00022024)
#define HWIO_TURING_TEST_BUS_SEL_RMSK                                                                                    0x3
#define HWIO_TURING_TEST_BUS_SEL_IN          \
        in_dword_masked(HWIO_TURING_TEST_BUS_SEL_ADDR, HWIO_TURING_TEST_BUS_SEL_RMSK)
#define HWIO_TURING_TEST_BUS_SEL_INM(m)      \
        in_dword_masked(HWIO_TURING_TEST_BUS_SEL_ADDR, m)
#define HWIO_TURING_TEST_BUS_SEL_OUT(v)      \
        out_dword(HWIO_TURING_TEST_BUS_SEL_ADDR,v)
#define HWIO_TURING_TEST_BUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TEST_BUS_SEL_ADDR,m,v,HWIO_TURING_TEST_BUS_SEL_IN)
#define HWIO_TURING_TEST_BUS_SEL_SEL_BMSK                                                                                0x3
#define HWIO_TURING_TEST_BUS_SEL_SEL_SHFT                                                                                0x0

#define HWIO_TURING_TURING_CC_SPARE_REG_ADDR                                                                      (TURING_CC_REG_BASE      + 0x00023000)
#define HWIO_TURING_TURING_CC_SPARE_REG_RMSK                                                                      0xffffffff
#define HWIO_TURING_TURING_CC_SPARE_REG_IN          \
        in_dword_masked(HWIO_TURING_TURING_CC_SPARE_REG_ADDR, HWIO_TURING_TURING_CC_SPARE_REG_RMSK)
#define HWIO_TURING_TURING_CC_SPARE_REG_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_CC_SPARE_REG_ADDR, m)
#define HWIO_TURING_TURING_CC_SPARE_REG_OUT(v)      \
        out_dword(HWIO_TURING_TURING_CC_SPARE_REG_ADDR,v)
#define HWIO_TURING_TURING_CC_SPARE_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_CC_SPARE_REG_ADDR,m,v,HWIO_TURING_TURING_CC_SPARE_REG_IN)
#define HWIO_TURING_TURING_CC_SPARE_REG_SPARE_BMSK                                                                0xfffffffe
#define HWIO_TURING_TURING_CC_SPARE_REG_SPARE_SHFT                                                                       0x1
#define HWIO_TURING_TURING_CC_SPARE_REG_ENABLE_VAPSS_GDS_CLK_DIS_Q6_AXIS2_CBC_BMSK                                       0x1
#define HWIO_TURING_TURING_CC_SPARE_REG_ENABLE_VAPSS_GDS_CLK_DIS_Q6_AXIS2_CBC_SHFT                                       0x0

#define HWIO_TURING_TURING_CC_SPARE1_REG_ADDR                                                                     (TURING_CC_REG_BASE      + 0x00023004)
#define HWIO_TURING_TURING_CC_SPARE1_REG_RMSK                                                                     0xffffffff
#define HWIO_TURING_TURING_CC_SPARE1_REG_IN          \
        in_dword_masked(HWIO_TURING_TURING_CC_SPARE1_REG_ADDR, HWIO_TURING_TURING_CC_SPARE1_REG_RMSK)
#define HWIO_TURING_TURING_CC_SPARE1_REG_INM(m)      \
        in_dword_masked(HWIO_TURING_TURING_CC_SPARE1_REG_ADDR, m)
#define HWIO_TURING_TURING_CC_SPARE1_REG_OUT(v)      \
        out_dword(HWIO_TURING_TURING_CC_SPARE1_REG_ADDR,v)
#define HWIO_TURING_TURING_CC_SPARE1_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_TURING_CC_SPARE1_REG_ADDR,m,v,HWIO_TURING_TURING_CC_SPARE1_REG_IN)
#define HWIO_TURING_TURING_CC_SPARE1_REG_SPARE1_BMSK                                                              0xffffffff
#define HWIO_TURING_TURING_CC_SPARE1_REG_SPARE1_SHFT                                                                     0x0


#endif /* __RSC_HALHWIO_H__ */