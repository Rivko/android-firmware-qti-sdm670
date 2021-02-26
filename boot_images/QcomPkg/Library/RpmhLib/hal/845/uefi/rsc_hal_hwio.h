#ifndef __RSC_HAL_HWIO_H__
#define __RSC_HAL_HWIO_H__
/*
===========================================================================
*/
/**
  @file rsc_hal_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p2q1r30.0.0_F2_ECO]
 
  This file contains HWIO register definitions for the following modules:
    APSS_RSC_RSCC_RSCC_RSC

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 

  Attribute definitions for the HWIO_*_ATTR macros are as follows:
    0x0: Command register
    0x1: Read-Only
    0x2: Write-Only
    0x3: Read/Write
*/
/*
  ===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/RpmhLib/hal/845/uefi/rsc_hal_hwio.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "HALhwio.h"
#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * BASE: APSS_HM
 *--------------------------------------------------------------------------*/

#define APSS_HM_BASE                                                0x17800000
#define APSS_HM_BASE_SIZE                                           0x00800000
#define APSS_HM_BASE_PHYS                                           0x17800000

/*----------------------------------------------------------------------------
 * MODULE: APSS_RSC_RSCC_RSCC_RSC
 *--------------------------------------------------------------------------*/

#define APSS_RSC_RSCC_RSCC_RSC_REG_BASE                                                                              (APSS_HM_BASE      + 0x001c0000)
#define APSS_RSC_RSCC_RSCC_RSC_REG_BASE_PHYS                                                                         (APSS_HM_BASE_PHYS + 0x001c0000)
#define APSS_RSC_RSCC_RSCC_RSC_REG_BASE_OFFS                                                                         0x001c0000

#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_ADDR(base,d)                                                                  ((base) + 0x00000000 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_PHYS(base,d)                                                                  ((base) + 0x00000000 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_OFFS(base,d)                                                                  (0x00000000 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_RMSK                                                                            0xffffff
#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_MAXd                                                                                   2
#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_POR                                                                           0x00010000
#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_POR_RMSK                                                                      0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_ATTR                                                                                 0x1
#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_MAJOR_VER_BMSK                                                                  0xff0000
#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_MAJOR_VER_SHFT                                                                      0x10
#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_MINOR_VER_BMSK                                                                    0xff00
#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_MINOR_VER_SHFT                                                                       0x8
#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_STEP_VER_BMSK                                                                       0xff
#define HWIO_APSS_RSC_RSCC_RSC_ID_DRVd_STEP_VER_SHFT                                                                        0x0

#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(base,d)                                                 ((base) + 0x00000004 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_PHYS(base,d)                                                 ((base) + 0x00000004 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_OFFS(base,d)                                                 (0x00000004 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_RMSK                                                          0x31f1f1f
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_MAXd                                                                  2
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_POR                                                          0x00010100
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_POR_RMSK                                                     0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_ATTR                                                                0x1
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_IS_CHILD_RSC_BMSK                                             0x2000000
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_IS_CHILD_RSC_SHFT                                                  0x19
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_IS_CHILD_RSC_PARENT_RSC_FVAL                                        0x0
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_IS_CHILD_RSC_CHILD_RSC_FVAL                                         0x1
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_HW_SOLVER_BMSK                                                0x1000000
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_HW_SOLVER_SHFT                                                     0x18
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_HW_SOLVER_SOLVER_FALSE_FVAL                                         0x0
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_HW_SOLVER_SOLVER_TRUE_FVAL                                          0x1
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_SLOTS_BMSK                                              0x1f0000
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_SLOTS_SHFT                                                  0x10
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_MODES_BMSK                                                0x1f00
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_MODES_SHFT                                                   0x8
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_NUM_TIMERS_BMSK                                                    0x1f
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_NUM_TIMERS_SHFT                                                     0x0

#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(base,d)                                                    ((base) + 0x00000008 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_PHYS(base,d)                                                    ((base) + 0x00000008 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_OFFS(base,d)                                                    (0x00000008 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_RMSK                                                             0x7ff0f1f
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_MAXd                                                                     2
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_POR                                                             0x03800414
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_POR_RMSK                                                        0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_ATTR                                                                   0x1
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_DRV_BMSK                                                     0x7000000
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_DRV_SHFT                                                          0x18
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_SEQ_CMD_WORDS_BMSK                                            0xff0000
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_SEQ_CMD_WORDS_SHFT                                                0x10
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_TS_EVENTS_BMSK                                                   0xf00
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_TS_EVENTS_SHFT                                                     0x8
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_DELAY_CNTR_BITWIDTH_BMSK                                              0x1f
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_DELAY_CNTR_BITWIDTH_SHFT                                               0x0

#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(base,d)                                        ((base) + 0x0000000c + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_PHYS(base,d)                                        ((base) + 0x0000000c + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_OFFS(base,d)                                        (0x0000000c + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_RMSK                                                0xf8ffffff
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_MAXd                                                         2
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_POR                                                 0x80008104
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_POR_RMSK                                            0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ATTR                                                       0x1
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_CMDS_PER_TCS_BMSK                               0xf8000000
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_CMDS_PER_TCS_SHFT                                     0x1b
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV3_BMSK                                     0xfc0000
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV3_SHFT                                         0x12
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV2_BMSK                                      0x3f000
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV2_SHFT                                          0xc
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV1_BMSK                                        0xfc0
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV1_SHFT                                          0x6
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV0_BMSK                                         0x3f
#define HWIO_APSS_RSC_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV0_SHFT                                          0x0

#define HWIO_APSS_RSC_RSCC_RSC_STATUS0_DRVd_ADDR(base,d)                                                             ((base) + 0x00000010 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_STATUS0_DRVd_PHYS(base,d)                                                             ((base) + 0x00000010 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_STATUS0_DRVd_OFFS(base,d)                                                             (0x00000010 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_STATUS0_DRVd_RMSK                                                                     0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_STATUS0_DRVd_MAXd                                                                              2
#define HWIO_APSS_RSC_RSCC_RSC_STATUS0_DRVd_POR                                                                      0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_STATUS0_DRVd_POR_RMSK                                                                 0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_STATUS0_DRVd_ATTR                                                                            0x1
#define HWIO_APSS_RSC_RSCC_RSC_STATUS0_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_STATUS0_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_STATUS0_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_STATUS0_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_STATUS0_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_STATUS0_DRVd_STATUS_SLEEP_CTL_FSM_BMSK                                                0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_STATUS0_DRVd_STATUS_SLEEP_CTL_FSM_SHFT                                                       0x0

#define HWIO_APSS_RSC_RSCC_RSC_STATUS1_DRVd_ADDR(base,d)                                                             ((base) + 0x00000014 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_STATUS1_DRVd_PHYS(base,d)                                                             ((base) + 0x00000014 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_STATUS1_DRVd_OFFS(base,d)                                                             (0x00000014 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_STATUS1_DRVd_RMSK                                                                     0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_STATUS1_DRVd_MAXd                                                                              2
#define HWIO_APSS_RSC_RSCC_RSC_STATUS1_DRVd_POR                                                                      0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_STATUS1_DRVd_POR_RMSK                                                                 0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_STATUS1_DRVd_ATTR                                                                            0x1
#define HWIO_APSS_RSC_RSCC_RSC_STATUS1_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_STATUS1_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_STATUS1_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_STATUS1_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_STATUS1_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_STATUS1_DRVd_STATUS_TBD_BMSK                                                          0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_STATUS1_DRVd_STATUS_TBD_SHFT                                                                 0x0

#define HWIO_APSS_RSC_RSCC_RSC_STATUS2_DRVd_ADDR(base,d)                                                             ((base) + 0x00000018 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_STATUS2_DRVd_PHYS(base,d)                                                             ((base) + 0x00000018 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_STATUS2_DRVd_OFFS(base,d)                                                             (0x00000018 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_STATUS2_DRVd_RMSK                                                                     0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_STATUS2_DRVd_MAXd                                                                              2
#define HWIO_APSS_RSC_RSCC_RSC_STATUS2_DRVd_POR                                                                      0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_STATUS2_DRVd_POR_RMSK                                                                 0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_STATUS2_DRVd_ATTR                                                                            0x1
#define HWIO_APSS_RSC_RSCC_RSC_STATUS2_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_STATUS2_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_STATUS2_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_STATUS2_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_STATUS2_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_STATUS2_DRVd_STATUS_TBD_BMSK                                                          0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_STATUS2_DRVd_STATUS_TBD_SHFT                                                                 0x0

#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_ADDR(x)                                                              ((x) + 0x0000001c)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_PHYS(x)                                                              ((x) + 0x0000001c)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_OFFS                                                                 (0x0000001c)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_RMSK                                                                    0x10003
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_POR                                                                  0x00000000
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_POR_RMSK                                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_ATTR                                                                        0x3
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_TRIGGER_BMSK                                                 0x10000
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_TRIGGER_SHFT                                                    0x10
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_PDC_TIMER_DRV_OWNER_BMSK                                         0x3
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_PDC_TIMER_DRV_OWNER_SHFT                                         0x0

#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR(x)                                                ((x) + 0x00000020)
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_PHYS(x)                                                ((x) + 0x00000020)
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_OFFS                                                   (0x00000020)
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_RMSK                                                       0xffff
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_POR                                                    0x00004520
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_POR_RMSK                                               0xffffffff
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ATTR                                                          0x3
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_PDC_SEQ_START_ADDR_REG_OFFSET_BMSK                         0xffff
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_PDC_SEQ_START_ADDR_REG_OFFSET_SHFT                            0x0

#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR(x)                                                ((x) + 0x00000024)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_PHYS(x)                                                ((x) + 0x00000024)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_OFFS                                                   (0x00000024)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_RMSK                                                       0xffff
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_POR                                                    0x00004510
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_POR_RMSK                                               0xffffffff
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ATTR                                                          0x3
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_PDC_MATCH_VALUE_LO_REG_OFFSET_BMSK                         0xffff
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_PDC_MATCH_VALUE_LO_REG_OFFSET_SHFT                            0x0

#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR(x)                                                ((x) + 0x00000028)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_PHYS(x)                                                ((x) + 0x00000028)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_OFFS                                                   (0x00000028)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_RMSK                                                       0xffff
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_POR                                                    0x00004514
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_POR_RMSK                                               0xffffffff
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ATTR                                                          0x3
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_PDC_MATCH_VALUE_HI_REG_OFFSET_BMSK                         0xffff
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_PDC_MATCH_VALUE_HI_REG_OFFSET_SHFT                            0x0

#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_ADDR(x)                                                                 ((x) + 0x0000002c)
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_PHYS(x)                                                                 ((x) + 0x0000002c)
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_OFFS                                                                    (0x0000002c)
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_RMSK                                                                           0x7
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_POR                                                                     0x00000001
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_POR_RMSK                                                                0xffffffff
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_ATTR                                                                           0x3
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_PDC_SLAVE_ID_BMSK                                                              0x7
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRV0_PDC_SLAVE_ID_SHFT                                                              0x0

#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRVd_ADDR(base,d)                                                         ((base) + 0x0000001c + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRVd_PHYS(base,d)                                                         ((base) + 0x0000001c + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRVd_OFFS(base,d)                                                         (0x0000001c + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRVd_RMSK                                                                        0x3
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRVd_MAXd                                                                          2
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRVd_POR                                                                  0x00000000
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRVd_POR_RMSK                                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRVd_ATTR                                                                        0x1
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRVd_HIDDEN_TCS_PDC_TIMER_DRV_OWNER_BMSK                                         0x3
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CTRL_DRVd_HIDDEN_TCS_PDC_TIMER_DRV_OWNER_SHFT                                         0x0

#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_ADDR(base,d)                                           ((base) + 0x00000020 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_PHYS(base,d)                                           ((base) + 0x00000020 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_OFFS(base,d)                                           (0x00000020 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_RMSK                                                       0xffff
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_MAXd                                                            2
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_POR                                                    0x00004520
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_POR_RMSK                                               0xffffffff
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_ATTR                                                          0x1
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_PDC_SEQ_START_ADDR_REG_OFFSET_BMSK                         0xffff
#define HWIO_APSS_RSC_RSCC_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_PDC_SEQ_START_ADDR_REG_OFFSET_SHFT                            0x0

#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_ADDR(base,d)                                           ((base) + 0x00000024 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_PHYS(base,d)                                           ((base) + 0x00000024 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_OFFS(base,d)                                           (0x00000024 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_RMSK                                                       0xffff
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_MAXd                                                            2
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_POR                                                    0x00004510
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_POR_RMSK                                               0xffffffff
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_ATTR                                                          0x1
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_PDC_MATCH_VALUE_LO_REG_OFFSET_BMSK                         0xffff
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_PDC_MATCH_VALUE_LO_REG_OFFSET_SHFT                            0x0

#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_ADDR(base,d)                                           ((base) + 0x00000028 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_PHYS(base,d)                                           ((base) + 0x00000028 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_OFFS(base,d)                                           (0x00000028 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_RMSK                                                       0xffff
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_MAXd                                                            2
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_POR                                                    0x00004514
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_POR_RMSK                                               0xffffffff
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_ATTR                                                          0x1
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_PDC_MATCH_VALUE_HI_REG_OFFSET_BMSK                         0xffff
#define HWIO_APSS_RSC_RSCC_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_PDC_MATCH_VALUE_HI_REG_OFFSET_SHFT                            0x0

#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRVd_ADDR(base,d)                                                            ((base) + 0x0000002c + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRVd_PHYS(base,d)                                                            ((base) + 0x0000002c + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRVd_OFFS(base,d)                                                            (0x0000002c + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRVd_RMSK                                                                           0x7
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRVd_MAXd                                                                             2
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRVd_POR                                                                     0x00000001
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRVd_POR_RMSK                                                                0xffffffff
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRVd_ATTR                                                                           0x1
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRVd_PDC_SLAVE_ID_BMSK                                                              0x7
#define HWIO_APSS_RSC_RSCC_PDC_SLAVE_ID_DRVd_PDC_SLAVE_ID_SHFT                                                              0x0

#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_ADDR(base,d)                                                       ((base) + 0x00000030 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_PHYS(base,d)                                                       ((base) + 0x00000030 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_OFFS(base,d)                                                       (0x00000030 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_RMSK                                                                  0x70707
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_MAXd                                                                        2
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_POR                                                                0x00000000
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_POR_RMSK                                                           0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_ATTR                                                                      0x1
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_COMPLETED_BMSK                                                        0x70000
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_COMPLETED_SHFT                                                           0x10
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_ISSUED_BMSK                                                             0x700
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_ISSUED_SHFT                                                               0x8
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_TRIGGERED_BMSK                                                            0x7
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_STATUS_DRVd_TRIGGERED_SHFT                                                            0x0

#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR(base,d)                                                    ((base) + 0x00000034 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_PHYS(base,d)                                                    ((base) + 0x00000034 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_OFFS(base,d)                                                    (0x00000034 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_RMSK                                                            0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_MAXd                                                                     2
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_POR                                                             0x00000000
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_POR_RMSK                                                        0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ATTR                                                                   0x1
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR_BMSK                                                       0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR_SHFT                                                              0x0

#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(base,d)                                                    ((base) + 0x00000038 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_PHYS(base,d)                                                    ((base) + 0x00000038 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_OFFS(base,d)                                                    (0x00000038 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_RMSK                                                            0x80ffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_MAXd                                                                     2
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_POR                                                             0x00000000
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_POR_RMSK                                                        0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ATTR                                                                   0x3
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_OUTI(base,d,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(base,d),val)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_OUTMI(base,d,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(base,d),mask,val,HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_INI(base,d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_VALID_BMSK                                      0x80000000
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_VALID_SHFT                                            0x1f
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_HI_BMSK                                           0xffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_HI_SHFT                                                0x0

#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR(base,d)                                                    ((base) + 0x0000003c + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_PHYS(base,d)                                                    ((base) + 0x0000003c + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_OFFS(base,d)                                                    (0x0000003c + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_RMSK                                                            0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_MAXd                                                                     2
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_POR                                                             0x00000000
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_POR_RMSK                                                        0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ATTR                                                                   0x1
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR_BMSK                                                       0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR_SHFT                                                              0x0

#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(base,d)                                                    ((base) + 0x00000040 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_PHYS(base,d)                                                    ((base) + 0x00000040 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_OFFS(base,d)                                                    (0x00000040 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_RMSK                                                            0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_MAXd                                                                     2
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_POR                                                             0x00000000
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_POR_RMSK                                                        0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ATTR                                                                   0x3
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_OUTI(base,d,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(base,d),val)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_OUTMI(base,d,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(base,d),mask,val,HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_INI(base,d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_PDC_MATCH_VALUE_LO_BMSK                                         0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD1_DATA_DRVd_PDC_MATCH_VALUE_LO_SHFT                                                0x0

#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR(base,d)                                                    ((base) + 0x00000044 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_PHYS(base,d)                                                    ((base) + 0x00000044 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_OFFS(base,d)                                                    (0x00000044 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_RMSK                                                            0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_MAXd                                                                     2
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_POR                                                             0x00000000
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_POR_RMSK                                                        0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ATTR                                                                   0x1
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR_BMSK                                                       0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR_SHFT                                                              0x0

#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR(x)                                                         ((x) + 0x00000048)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_PHYS(x)                                                         ((x) + 0x00000048)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_OFFS                                                            (0x00000048)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_RMSK                                                            0x8000ffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_POR                                                             0x00000000
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_POR_RMSK                                                        0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ATTR                                                                   0x3
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_VALID_BMSK                                   0x80000000
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_VALID_SHFT                                         0x1f
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_BMSK                                             0xffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_SHFT                                                0x0

#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_ADDR(base,d)                                                    ((base) + 0x00000048 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_PHYS(base,d)                                                    ((base) + 0x00000048 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_OFFS(base,d)                                                    (0x00000048 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_RMSK                                                            0x8000ffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_MAXd                                                                     2
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_POR                                                             0x00000000
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_POR_RMSK                                                        0xffffffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_ATTR                                                                   0x1
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_PDC_SEQ_START_ADDR_VALID_BMSK                                   0x80000000
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_PDC_SEQ_START_ADDR_VALID_SHFT                                         0x1f
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_PDC_SEQ_START_ADDR_BMSK                                             0xffff
#define HWIO_APSS_RSC_RSCC_HIDDEN_TCS_CMD2_DATA_DRVd_PDC_SEQ_START_ADDR_SHFT                                                0x0

#define HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_ADDR(x)                                                               ((x) + 0x0000004c)
#define HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_PHYS(x)                                                               ((x) + 0x0000004c)
#define HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_OFFS                                                                  (0x0000004c)
#define HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_RMSK                                                                         0x3
#define HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_POR                                                                   0x00000000
#define HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_POR_RMSK                                                              0xffffffff
#define HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_ATTR                                                                         0x3
#define HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_OWNER_BMSK                                                                   0x3
#define HWIO_APSS_RSC_RSCC_HW_EVENT_OWNER_DRV0_OWNER_SHFT                                                                   0x0

#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(base,d,m)                                                  ((base) + 0x00000050 + 0x4 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_PHYS(base,d,m)                                                  ((base) + 0x00000050 + 0x4 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_OFFS(base,d,m)                                                  (0x00000050 + 0x4 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_RMSK                                                            0x80000007
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_MAXd                                                                     2
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_MAXm                                                                    31
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_POR                                                             0x00000000
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_POR_RMSK                                                        0xffffffff
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ATTR                                                                   0x3
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(base,d,m), HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(base,d,m), mask)
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_OUTI2(base,d,m,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(base,d,m),val)
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_OUTMI2(base,d,m,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(base,d,m),mask,val,HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_INI2(base,d,m))
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ENABLE_BMSK                                                     0x80000000
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ENABLE_SHFT                                                           0x1f
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ENABLE_NOT_ENABLED_FVAL                                                0x0
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ENABLE_ENABLED_FVAL                                                    0x1
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_EVENT_SELECT_BMSK                                                      0x7
#define HWIO_APSS_RSC_RSCC_HW_EVENT_MUXm_SELECT_DRVd_EVENT_SELECT_SHFT                                                      0x0

#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_ADDR(x)                                                         ((x) + 0x000000d0)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_PHYS(x)                                                         ((x) + 0x000000d0)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_OFFS                                                            (0x000000d0)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_RMSK                                                                   0x1
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_POR                                                             0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_POR_RMSK                                                        0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_ATTR                                                                   0x1
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_EPCB_TIMEOUT_IRQ_BMSK                                                  0x1
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_EPCB_TIMEOUT_IRQ_SHFT                                                  0x0

#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_ADDR(x)                                                          ((x) + 0x000000d4)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_PHYS(x)                                                          ((x) + 0x000000d4)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_OFFS                                                             (0x000000d4)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_RMSK                                                                    0x1
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_POR                                                              0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_POR_RMSK                                                         0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_ATTR                                                                    0x0
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_CLR_EPCB_TIMEOUT_IRQ_BMSK                                               0x1
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_CLR_EPCB_TIMEOUT_IRQ_SHFT                                               0x0

#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR(x)                                                         ((x) + 0x000000d8)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_PHYS(x)                                                         ((x) + 0x000000d8)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_OFFS                                                            (0x000000d8)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_RMSK                                                              0x10ffff
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_POR                                                             0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_POR_RMSK                                                        0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ATTR                                                                   0x3
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_IRQ_EN_BMSK                                          0x100000
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_IRQ_EN_SHFT                                              0x14
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_IRQ_EN_NOT_ENABLED_FVAL                                   0x0
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_IRQ_EN_ENABLED_FVAL                                       0x1
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_THRESHOLD_BMSK                                         0xffff
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_THRESHOLD_SHFT                                            0x0

#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRVd_ADDR(base,d)                                                    ((base) + 0x000000d0 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRVd_PHYS(base,d)                                                    ((base) + 0x000000d0 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRVd_OFFS(base,d)                                                    (0x000000d0 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRVd_RMSK                                                                   0x1
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRVd_MAXd                                                                     2
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRVd_POR                                                             0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRVd_POR_RMSK                                                        0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRVd_ATTR                                                                   0x1
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRVd_EPCB_TIMEOUT_IRQ_BMSK                                                  0x1
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_STATUS_DRVd_EPCB_TIMEOUT_IRQ_SHFT                                                  0x0

#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_ADDR(base,d)                                                    ((base) + 0x000000d8 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_PHYS(base,d)                                                    ((base) + 0x000000d8 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_OFFS(base,d)                                                    (0x000000d8 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_RMSK                                                              0x10ffff
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_MAXd                                                                     2
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_POR                                                             0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_POR_RMSK                                                        0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_ATTR                                                                   0x1
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_EPCB_TIMEOUT_IRQ_EN_BMSK                                          0x100000
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_EPCB_TIMEOUT_IRQ_EN_SHFT                                              0x14
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_EPCB_TIMEOUT_IRQ_EN_NOT_ENABLED_FVAL                                   0x0
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_EPCB_TIMEOUT_IRQ_EN_ENABLED_FVAL                                       0x1
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_EPCB_TIMEOUT_THRESHOLD_BMSK                                         0xffff
#define HWIO_APSS_RSC_RSCC_RSC_ERROR_IRQ_ENABLE_DRVd_EPCB_TIMEOUT_THRESHOLD_SHFT                                            0x0

#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR(x)                                                     ((x) + 0x00000200)
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_PHYS(x)                                                     ((x) + 0x00000200)
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OFFS                                                        (0x00000200)
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_RMSK                                                               0x3
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_POR                                                         0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_POR_RMSK                                                    0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ATTR                                                               0x3
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OWNER_BMSK                                                         0x3
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OWNER_SHFT                                                         0x0

#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(base,d,m)                                                ((base) + 0x00000204 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_PHYS(base,d,m)                                                ((base) + 0x00000204 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_OFFS(base,d,m)                                                (0x00000204 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_RMSK                                                                 0x1
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_MAXd                                                                   2
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_MAXm                                                                   7
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_POR                                                           0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_POR_RMSK                                                      0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ATTR                                                                 0x3
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(base,d,m), HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(base,d,m), mask)
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_OUTI2(base,d,m,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(base,d,m),val)
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_OUTMI2(base,d,m,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(base,d,m),mask,val,HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_INI2(base,d,m))
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ENABLE_BMSK                                                          0x1
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ENABLE_SHFT                                                          0x0
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ENABLE_NOT_ENABLED_FVAL                                              0x0
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ENABLE_ENABLED_FVAL                                                  0x1

#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(base,d,m)                                       ((base) + 0x00000208 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_PHYS(base,d,m)                                       ((base) + 0x00000208 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_OFFS(base,d,m)                                       (0x00000208 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_RMSK                                                 0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_MAXd                                                          2
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_MAXm                                                          7
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_POR                                                  0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_POR_RMSK                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ATTR                                                        0x1
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(base,d,m), HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(base,d,m), mask)
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_TS_DATA_L_BMSK                                       0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_TS_DATA_L_SHFT                                              0x0

#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(base,d,m)                                       ((base) + 0x0000020c + 0x20 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_PHYS(base,d,m)                                       ((base) + 0x0000020c + 0x20 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_OFFS(base,d,m)                                       (0x0000020c + 0x20 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_RMSK                                                   0xffffff
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_MAXd                                                          2
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_MAXm                                                          7
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_POR                                                  0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_POR_RMSK                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ATTR                                                        0x1
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(base,d,m), HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(base,d,m), mask)
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_TS_DATA_H_BMSK                                         0xffffff
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_TS_DATA_H_SHFT                                              0x0

#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(base,d,m)                                            ((base) + 0x00000210 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_PHYS(base,d,m)                                            ((base) + 0x00000210 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_OFFS(base,d,m)                                            (0x00000210 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_RMSK                                                           0x101
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_MAXd                                                               2
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_MAXm                                                               7
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_POR                                                       0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_POR_RMSK                                                  0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ATTR                                                             0x1
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(base,d,m), HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(base,d,m), mask)
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_OVERFLOW_BMSK                                               0x100
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_OVERFLOW_SHFT                                                 0x8
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_OVERFLOW_MULTIPLE_EVENT_CAPTURES_FALSE_FVAL                   0x0
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_OVERFLOW_MULTIPLE_EVENT_CAPTURES_TRUE_FVAL                    0x1
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_VALID_BMSK                                                    0x1
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_VALID_SHFT                                                    0x0
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_VALID_EVENT_CAPTURED_FALSE_FVAL                               0x0
#define HWIO_APSS_RSC_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_VALID_EVENT_CAPTURED_TRUE_FVAL                                0x1

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR(x)                                                  ((x) + 0x00000400)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_PHYS(x)                                                  ((x) + 0x00000400)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_OFFS                                                     (0x00000400)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_RMSK                                                     0x800003ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_POR                                                      0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_POR_RMSK                                                 0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ATTR                                                            0x3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_VALID_BMSK                                               0x80000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_VALID_SHFT                                                     0x1f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR_BMSK                                                     0x3ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR_SHFT                                                       0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRV0_ADDR(x)                                                                 ((x) + 0x00000404)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRV0_PHYS(x)                                                                 ((x) + 0x00000404)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRV0_OFFS                                                                    (0x00000404)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRV0_RMSK                                                                           0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRV0_POR                                                                     0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRV0_POR_RMSK                                                                0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRV0_ATTR                                                                           0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRV0_BUSY_STAT_BMSK                                                                 0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRV0_BUSY_STAT_SHFT                                                                 0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR(x)                                                      ((x) + 0x00000408)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_PHYS(x)                                                      ((x) + 0x00000408)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_OFFS                                                         (0x00000408)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_RMSK                                                              0x3ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_POR                                                          0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_POR_RMSK                                                     0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_ATTR                                                                0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_CURRENT_INSTR_ADDR_BMSK                                           0x3ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_CURRENT_INSTR_ADDR_SHFT                                             0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(base,i)                                                   ((base) + 0x00000410 + 0x4 * (i))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_PHYS(base,i)                                                   ((base) + 0x00000410 + 0x4 * (i))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_OFFS(base,i)                                                   (0x00000410 + 0x4 * (i))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_RMSK                                                                0x3ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_MAXi                                                                    3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_POR                                                            0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_POR_RMSK                                                       0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ATTR                                                                  0x3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INI(base,i)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(base,i), HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INMI(base,i,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(base,i), mask)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_OUTI(base,i,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(base,i),val)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_OUTMI(base,i,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(base,i),mask,val,HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INI(base,i))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR_BMSK                                                           0x3ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR_SHFT                                                             0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(base,i)                                                 ((base) + 0x00000450 + 0x4 * (i))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_PHYS(base,i)                                                 ((base) + 0x00000450 + 0x4 * (i))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_OFFS(base,i)                                                 (0x00000450 + 0x4 * (i))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_RMSK                                                            0xfffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_MAXi                                                                  3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_POR                                                          0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_POR_RMSK                                                     0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ATTR                                                                0x3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INI(base,i)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(base,i), HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INMI(base,i,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(base,i), mask)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_OUTI(base,i,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(base,i),val)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_OUTMI(base,i,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(base,i),mask,val,HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INI(base,i))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_DELAY_VAL_BMSK                                                  0xfffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_DELAY_VAL_SHFT                                                      0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR(x)                                                     ((x) + 0x00000460)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_PHYS(x)                                                     ((x) + 0x00000460)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_OFFS                                                        (0x00000460)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_RMSK                                                               0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_POR                                                         0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_POR_RMSK                                                    0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ATTR                                                               0x3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_SEQ_OVERRIDE_TRIGGER_BMSK                                          0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_SEQ_OVERRIDE_TRIGGER_SHFT                                          0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR(x)                                       ((x) + 0x00000464)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_PHYS(x)                                       ((x) + 0x00000464)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_OFFS                                          (0x00000464)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_RMSK                                               0x3ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_POR                                           0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_POR_RMSK                                      0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ATTR                                                 0x3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_SEQ_OVERRIDE_TRIGGER_START_ADDR_BMSK               0x3ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_SEQ_OVERRIDE_TRIGGER_START_ADDR_SHFT                 0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_ADDR(base,d)                                             ((base) + 0x00000400 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_PHYS(base,d)                                             ((base) + 0x00000400 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_OFFS(base,d)                                             (0x00000400 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_RMSK                                                     0x800003ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_MAXd                                                              2
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_POR                                                      0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_POR_RMSK                                                 0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_ATTR                                                            0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_VALID_BMSK                                               0x80000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_VALID_SHFT                                                     0x1f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_ADDR_BMSK                                                     0x3ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_ADDR_SHFT                                                       0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRVd_ADDR(base,d)                                                            ((base) + 0x00000404 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRVd_PHYS(base,d)                                                            ((base) + 0x00000404 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRVd_OFFS(base,d)                                                            (0x00000404 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRVd_RMSK                                                                           0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRVd_MAXd                                                                             2
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRVd_POR                                                                     0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRVd_POR_RMSK                                                                0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRVd_ATTR                                                                           0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRVd_BUSY_STAT_BMSK                                                                 0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_BUSY_DRVd_BUSY_STAT_SHFT                                                                 0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRVd_ADDR(base,d)                                                 ((base) + 0x00000408 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRVd_PHYS(base,d)                                                 ((base) + 0x00000408 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRVd_OFFS(base,d)                                                 (0x00000408 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRVd_RMSK                                                              0x3ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRVd_MAXd                                                                  2
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRVd_POR                                                          0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRVd_POR_RMSK                                                     0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRVd_ATTR                                                                0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRVd_CURRENT_INSTR_ADDR_BMSK                                           0x3ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRVd_CURRENT_INSTR_ADDR_SHFT                                             0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_ADDR(base,d,i)                                                 ((base) + 0x00000410 + 0x4 * (i) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_PHYS(base,d,i)                                                 ((base) + 0x00000410 + 0x4 * (i) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_OFFS(base,d,i)                                                 (0x00000410 + 0x4 * (i) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_RMSK                                                                0x3ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_MAXd                                                                    2
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_MAXi                                                                    3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_POR                                                            0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_POR_RMSK                                                       0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_ATTR                                                                  0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_INI2(base,d,i)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_ADDR(base,d,i), HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_INMI2(base,d,i,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_ADDR(base,d,i), mask)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_ADDR_BMSK                                                           0x3ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRVd_ADDR_SHFT                                                             0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_ADDR(base,d,i)                                               ((base) + 0x00000450 + 0x4 * (i) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_PHYS(base,d,i)                                               ((base) + 0x00000450 + 0x4 * (i) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_OFFS(base,d,i)                                               (0x00000450 + 0x4 * (i) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_RMSK                                                            0xfffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_MAXd                                                                  2
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_MAXi                                                                  3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_POR                                                          0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_POR_RMSK                                                     0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_ATTR                                                                0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_INI2(base,d,i)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_ADDR(base,d,i), HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_INMI2(base,d,i,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_ADDR(base,d,i), mask)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_DELAY_VAL_BMSK                                                  0xfffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_DELAY_VAL_SHFT                                                      0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_ADDR(base,d)                                                ((base) + 0x00000460 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_PHYS(base,d)                                                ((base) + 0x00000460 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_OFFS(base,d)                                                (0x00000460 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_RMSK                                                               0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_MAXd                                                                 2
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_POR                                                         0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_POR_RMSK                                                    0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_ATTR                                                               0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_SEQ_OVERRIDE_TRIGGER_BMSK                                          0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_SEQ_OVERRIDE_TRIGGER_SHFT                                          0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_ADDR(base,d)                                  ((base) + 0x00000464 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_PHYS(base,d)                                  ((base) + 0x00000464 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_OFFS(base,d)                                  (0x00000464 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_RMSK                                               0x3ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_MAXd                                                   2
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_POR                                           0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_POR_RMSK                                      0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_ATTR                                                 0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_SEQ_OVERRIDE_TRIGGER_START_ADDR_BMSK               0x3ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_SEQ_OVERRIDE_TRIGGER_START_ADDR_SHFT                 0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR(x)                                                  ((x) + 0x00000490)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_PHYS(x)                                                  ((x) + 0x00000490)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_OFFS                                                     (0x00000490)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_RMSK                                                     0x800003ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_POR                                                      0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_POR_RMSK                                                 0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ATTR                                                            0x3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_VALID_BMSK                                0x80000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_VALID_SHFT                                      0x1f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_ADDR_BMSK                                      0x3ff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_ADDR_SHFT                                        0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR(x)                                                             ((x) + 0x00000494)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_PHYS(x)                                                             ((x) + 0x00000494)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_OFFS                                                                (0x00000494)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_RMSK                                                                       0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_POR                                                                 0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_POR_RMSK                                                            0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_ATTR                                                                       0x3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_DBG_STEP_TRIGGER_BMSK                                                      0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STEP_DRV0_DBG_STEP_TRIGGER_SHFT                                                      0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR(x)                                                         ((x) + 0x00000498)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_PHYS(x)                                                         ((x) + 0x00000498)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_OFFS                                                            (0x00000498)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_RMSK                                                                   0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_POR                                                             0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_POR_RMSK                                                        0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ATTR                                                                   0x3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_DBG_STEP_CONTINUE_BMSK                                                 0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_DBG_STEP_CONTINUE_SHFT                                                 0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_ADDR(x)                                                             ((x) + 0x0000049c)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_PHYS(x)                                                             ((x) + 0x0000049c)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_OFFS                                                                (0x0000049c)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_RMSK                                                                       0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_POR                                                                 0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_POR_RMSK                                                            0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_ATTR                                                                       0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_DBG_STEPPING_STATUS_BMSK                                                   0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_DBG_STAT_DRV0_DBG_STEPPING_STATUS_SHFT                                                   0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR(x)                                              ((x) + 0x000004a0)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_PHYS(x)                                              ((x) + 0x000004a0)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_OFFS                                                 (0x000004a0)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_RMSK                                                       0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_POR                                                  0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_POR_RMSK                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ATTR                                                        0x3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_SEQ_OVERRIDE_PWR_CNTRL_MASK_BMSK                           0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_SEQ_OVERRIDE_PWR_CNTRL_MASK_SHFT                            0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR(x)                                               ((x) + 0x000004a4)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_PHYS(x)                                               ((x) + 0x000004a4)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_OFFS                                                  (0x000004a4)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_RMSK                                                        0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_POR                                                   0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_POR_RMSK                                              0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ATTR                                                         0x3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_SEQ_OVERRIDE_PWR_CNTRL_VAL_BMSK                             0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_SEQ_OVERRIDE_PWR_CNTRL_VAL_SHFT                              0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR(x)                                             ((x) + 0x000004a8)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_PHYS(x)                                             ((x) + 0x000004a8)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_OFFS                                                (0x000004a8)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_RMSK                                                      0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_POR                                                 0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_POR_RMSK                                            0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ATTR                                                       0x3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_SEQ_OVERRIDE_WAIT_EVENT_MASK_BMSK                         0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_SEQ_OVERRIDE_WAIT_EVENT_MASK_SHFT                          0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR(x)                                              ((x) + 0x000004ac)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_PHYS(x)                                              ((x) + 0x000004ac)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_OFFS                                                 (0x000004ac)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_RMSK                                                       0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_POR                                                  0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_POR_RMSK                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ATTR                                                        0x3
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_SEQ_OVERRIDE_WAIT_EVENT_VAL_BMSK                           0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_SEQ_OVERRIDE_WAIT_EVENT_VAL_SHFT                            0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_ADDR(base,d)                                         ((base) + 0x000004a0 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_PHYS(base,d)                                         ((base) + 0x000004a0 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_OFFS(base,d)                                         (0x000004a0 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_RMSK                                                       0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_MAXd                                                          2
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_POR                                                  0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_POR_RMSK                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_ATTR                                                        0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_SEQ_OVERRIDE_PWR_CNTRL_MASK_BMSK                           0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_SEQ_OVERRIDE_PWR_CNTRL_MASK_SHFT                            0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_ADDR(base,d)                                          ((base) + 0x000004a4 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_PHYS(base,d)                                          ((base) + 0x000004a4 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_OFFS(base,d)                                          (0x000004a4 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_RMSK                                                        0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_MAXd                                                           2
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_POR                                                   0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_POR_RMSK                                              0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_ATTR                                                         0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_SEQ_OVERRIDE_PWR_CNTRL_VAL_BMSK                             0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_SEQ_OVERRIDE_PWR_CNTRL_VAL_SHFT                              0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_ADDR(base,d)                                        ((base) + 0x000004a8 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_PHYS(base,d)                                        ((base) + 0x000004a8 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_OFFS(base,d)                                        (0x000004a8 + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_RMSK                                                      0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_MAXd                                                         2
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_POR                                                 0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_POR_RMSK                                            0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_ATTR                                                       0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_SEQ_OVERRIDE_WAIT_EVENT_MASK_BMSK                         0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_SEQ_OVERRIDE_WAIT_EVENT_MASK_SHFT                          0x0

#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_ADDR(base,d)                                         ((base) + 0x000004ac + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_PHYS(base,d)                                         ((base) + 0x000004ac + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_OFFS(base,d)                                         (0x000004ac + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_RMSK                                                       0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_MAXd                                                          2
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_POR                                                  0x00000000
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_POR_RMSK                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_ATTR                                                        0x1
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_ADDR(base,d), HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_ADDR(base,d), mask)
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_SEQ_OVERRIDE_WAIT_EVENT_VAL_BMSK                           0x3f
#define HWIO_APSS_RSC_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_SEQ_OVERRIDE_WAIT_EVENT_VAL_SHFT                            0x0

#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_ADDR(base,m)                                                               ((base) + 0x00000600 + 0x4 * (m))
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_PHYS(base,m)                                                               ((base) + 0x00000600 + 0x4 * (m))
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_OFFS(base,m)                                                               (0x00000600 + 0x4 * (m))
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_RMSK                                                                       0xffffffff
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_MAXm                                                                              127
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_POR                                                                        0x00000000
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_POR_RMSK                                                                   0xffffffff
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_ATTR                                                                              0x3
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_INI(base,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_ADDR(base,m), HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_INMI(base,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_ADDR(base,m), mask)
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_OUTI(base,m,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_ADDR(base,m),val)
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_ADDR(base,m),mask,val,HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_INI(base,m))
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_CMD_WORD_BMSK                                                              0xffffffff
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRV0_CMD_WORD_SHFT                                                                     0x0

#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_ADDR(base,d,m)                                                             ((base) + 0x00000600 + 0x4 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_PHYS(base,d,m)                                                             ((base) + 0x00000600 + 0x4 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_OFFS(base,d,m)                                                             (0x00000600 + 0x4 * (m) + 0x10000 * (d))
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_RMSK                                                                       0xffffffff
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_MAXd                                                                                2
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_MAXm                                                                              127
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_POR                                                                        0x00000000
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_POR_RMSK                                                                   0xffffffff
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_ATTR                                                                              0x1
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_ADDR(base,d,m), HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_RMSK)
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_ADDR(base,d,m), mask)
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_CMD_WORD_BMSK                                                              0xffffffff
#define HWIO_APSS_RSC_RSCC_SEQ_MEM_m_DRVd_CMD_WORD_SHFT                                                                     0x0

#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR(x)                                                      ((x) + 0x00000d00)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_PHYS(x)                                                      ((x) + 0x00000d00)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_OFFS                                                         (0x00000d00)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_RMSK                                                                0xf
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_POR                                                          0x00000000
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_POR_RMSK                                                     0xffffffff
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ATTR                                                                0x3
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_IN(x))
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_AMC_COMPLETION_IRQ_ENABLE_BMSK                                      0xf
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0_AMC_COMPLETION_IRQ_ENABLE_SHFT                                      0x0

#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_ADDR(x)                                                      ((x) + 0x00000d04)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_PHYS(x)                                                      ((x) + 0x00000d04)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_OFFS                                                         (0x00000d04)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_RMSK                                                                0xf
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_POR                                                          0x00000000
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_POR_RMSK                                                     0xffffffff
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_ATTR                                                                0x1
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_ADDR(x), HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_RMSK)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_AMC_COMPLETION_IRQ_STATUS_BMSK                                      0xf
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0_AMC_COMPLETION_IRQ_STATUS_SHFT                                      0x0

#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_ADDR(x)                                                       ((x) + 0x00000d08)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_PHYS(x)                                                       ((x) + 0x00000d08)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_OFFS                                                          (0x00000d08)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_RMSK                                                                 0xf
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_POR                                                           0x00000000
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_POR_RMSK                                                      0xffffffff
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_ATTR                                                                 0x0
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_AMC_COMPLETION_IRQ_CLEAR_BMSK                                        0xf
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0_AMC_COMPLETION_IRQ_CLEAR_SHFT                                        0x0

#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(base,m)                                                  ((base) + 0x00000d10 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_PHYS(base,m)                                                  ((base) + 0x00000d10 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_OFFS(base,m)                                                  (0x00000d10 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_RMSK                                                              0xffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_MAXm                                                                   3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_POR                                                           0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_POR_RMSK                                                      0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ATTR                                                                 0x3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_INI(base,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(base,m), HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_INMI(base,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(base,m), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_OUTI(base,m,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(base,m),val)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(base,m),mask,val,HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_INI(base,m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_BMSK                                            0xffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_SHFT                                               0x0

#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_ADDR(base,m)                                                            ((base) + 0x00000d14 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_PHYS(base,m)                                                            ((base) + 0x00000d14 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_OFFS(base,m)                                                            (0x00000d14 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_RMSK                                                                     0x1010000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_MAXm                                                                             3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_POR                                                                     0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_POR_RMSK                                                                0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_ATTR                                                                           0x3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_INI(base,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_ADDR(base,m), HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_INMI(base,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_ADDR(base,m), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_OUTI(base,m,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_ADDR(base,m),val)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_ADDR(base,m),mask,val,HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_INI(base,m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_AMC_MODE_TRIGGER_BMSK                                                    0x1000000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_AMC_MODE_TRIGGER_SHFT                                                         0x18
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_AMC_MODE_EN_BMSK                                                           0x10000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_AMC_MODE_EN_SHFT                                                              0x10
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_AMC_MODE_EN_AMC_MODE_DISABLE_FVAL                                              0x0
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CONTROL_AMC_MODE_EN_AMC_MODE_ENABLED_FVAL                                              0x1

#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_ADDR(base,m)                                                             ((base) + 0x00000d18 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_PHYS(base,m)                                                             ((base) + 0x00000d18 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_OFFS(base,m)                                                             (0x00000d18 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_RMSK                                                                            0x1
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_MAXm                                                                              3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_POR                                                                      0x00000001
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_POR_RMSK                                                                 0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_ATTR                                                                            0x1
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_INI(base,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_ADDR(base,m), HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_INMI(base,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_ADDR(base,m), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_CONTROLLER_IDLE_BMSK                                                            0x1
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_CONTROLLER_IDLE_SHFT                                                            0x0
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_CONTROLLER_IDLE_NOT_IDLE_FVAL                                                   0x0
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_STATUS_CONTROLLER_IDLE_IS_IDLE_FVAL                                                    0x1

#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_ADDR(base,m)                                                         ((base) + 0x00000d1c + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_PHYS(base,m)                                                         ((base) + 0x00000d1c + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_OFFS(base,m)                                                         (0x00000d1c + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_RMSK                                                                     0xffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_MAXm                                                                          3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_POR                                                                  0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_POR_RMSK                                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_ATTR                                                                        0x3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_INI(base,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_ADDR(base,m), HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_INMI(base,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_ADDR(base,m), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_OUTI(base,m,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_ADDR(base,m),val)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_ADDR(base,m),mask,val,HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_INI(base,m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_INDIVIDUAL_CMD_ENABLE_MASK_BMSK                                          0xffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV0_CMD_ENABLE_INDIVIDUAL_CMD_ENABLE_MASK_SHFT                                             0x0

#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_ADDR(base,m,n)                                                       ((base) + 0x00000d30 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_PHYS(base,m,n)                                                       ((base) + 0x00000d30 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_OFFS(base,m,n)                                                       (0x00000d30 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_RMSK                                                                    0x1010f
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_MAXm                                                                          3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_MAXn                                                                         15
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_POR                                                                  0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_POR_RMSK                                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_ATTR                                                                        0x3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_INI2(base,m,n)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_ADDR(base,m,n), HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_ADDR(base,m,n), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_OUTI2(base,m,n,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_ADDR(base,m,n),val)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_OUTMI2(base,m,n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_ADDR(base,m,n),mask,val,HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_INI2(base,m,n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_READ_OR_WRITE_BMSK                                                      0x10000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_READ_OR_WRITE_SHFT                                                         0x10
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_READ_OR_WRITE_READ_FVAL                                                     0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_READ_OR_WRITE_WRITE_FVAL                                                    0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_RES_REQ_BMSK                                                              0x100
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_RES_REQ_SHFT                                                                0x8
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_RES_REQ_FIRE_AND_FORGET_FVAL                                                0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_RES_REQ_RESPONSE_REQUIRED_FVAL                                              0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_MSG_LENGTH_BMSK                                                             0xf
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_MSGID_MSG_LENGTH_SHFT                                                             0x0

#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_ADDR(base,m,n)                                                        ((base) + 0x00000d34 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_PHYS(base,m,n)                                                        ((base) + 0x00000d34 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_OFFS(base,m,n)                                                        (0x00000d34 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_RMSK                                                                     0x7ffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_MAXm                                                                           3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_MAXn                                                                          15
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_POR                                                                   0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_POR_RMSK                                                              0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_ATTR                                                                         0x3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_INI2(base,m,n)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_ADDR(base,m,n), HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_ADDR(base,m,n), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_OUTI2(base,m,n,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_ADDR(base,m,n),val)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_OUTMI2(base,m,n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_ADDR(base,m,n),mask,val,HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_INI2(base,m,n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_SLV_ID_BMSK                                                              0x70000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_SLV_ID_SHFT                                                                 0x10
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_OFFSET_BMSK                                                               0xffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_ADDR_OFFSET_SHFT                                                                  0x0

#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_ADDR(base,m,n)                                                        ((base) + 0x00000d38 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_PHYS(base,m,n)                                                        ((base) + 0x00000d38 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_OFFS(base,m,n)                                                        (0x00000d38 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_RMSK                                                                  0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_MAXm                                                                           3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_MAXn                                                                          15
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_POR                                                                   0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_POR_RMSK                                                              0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_ATTR                                                                         0x3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_INI2(base,m,n)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_ADDR(base,m,n), HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_ADDR(base,m,n), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_OUTI2(base,m,n,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_ADDR(base,m,n),val)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_OUTMI2(base,m,n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_ADDR(base,m,n),mask,val,HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_INI2(base,m,n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_DATA_BMSK                                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_DATA_DATA_SHFT                                                                    0x0

#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_ADDR(base,m,n)                                                      ((base) + 0x00000d3c + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_PHYS(base,m,n)                                                      ((base) + 0x00000d3c + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_OFFS(base,m,n)                                                      (0x00000d3c + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_RMSK                                                                   0x10101
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_MAXm                                                                         3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_MAXn                                                                        15
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_POR                                                                 0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_POR_RMSK                                                            0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_ATTR                                                                       0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_INI2(base,m,n)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_ADDR(base,m,n), HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_ADDR(base,m,n), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_COMPLETED_BMSK                                                         0x10000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_COMPLETED_SHFT                                                            0x10
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_COMPLETED_HAS_NOT_BEEN_COMPLETE_FVAL                                       0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_COMPLETED_HAS_COMPLETED_FVAL                                               0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_ISSUED_BMSK                                                              0x100
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_ISSUED_SHFT                                                                0x8
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_ISSUED_HAS_NOT_BEEN_SENT_OVER_EPCB_FVAL                                    0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_ISSUED_HAS_BEEN_SENT_OVER_EPCB_FVAL                                        0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_TRIGGERED_BMSK                                                             0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_TRIGGERED_SHFT                                                             0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_TRIGGERED_HAS_NOT_BEEN_TRIGGERE_FVAL                                       0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_STATUS_TRIGGERED_HAS_BEEN_TRIGGERED_FVAL                                          0x1

#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_ADDR(base,m,n)                                          ((base) + 0x00000d40 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_PHYS(base,m,n)                                          ((base) + 0x00000d40 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_OFFS(base,m,n)                                          (0x00000d40 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_RMSK                                                    0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_MAXm                                                             3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_MAXn                                                            15
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_POR                                                     0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_POR_RMSK                                                0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_ATTR                                                           0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_INI2(base,m,n)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_ADDR(base,m,n), HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_ADDR(base,m,n), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_READ_RESPONSE_DATA_BMSK                                 0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_READ_RESPONSE_DATA_SHFT                                        0x0

#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_ADDR(x)                                                      ((x) + 0x00010d00)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_PHYS(x)                                                      ((x) + 0x00010d00)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_OFFS                                                         (0x00010d00)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_RMSK                                                                0xf
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_POR                                                          0x00000000
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_POR_RMSK                                                     0xffffffff
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_ATTR                                                                0x3
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_ADDR(x), HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_RMSK)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_IN(x))
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_AMC_COMPLETION_IRQ_ENABLE_BMSK                                      0xf
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1_AMC_COMPLETION_IRQ_ENABLE_SHFT                                      0x0

#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1_ADDR(x)                                                      ((x) + 0x00010d04)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1_PHYS(x)                                                      ((x) + 0x00010d04)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1_OFFS                                                         (0x00010d04)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1_RMSK                                                                0xf
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1_POR                                                          0x00000000
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1_POR_RMSK                                                     0xffffffff
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1_ATTR                                                                0x1
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1_ADDR(x), HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1_RMSK)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1_AMC_COMPLETION_IRQ_STATUS_BMSK                                      0xf
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1_AMC_COMPLETION_IRQ_STATUS_SHFT                                      0x0

#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV1_ADDR(x)                                                       ((x) + 0x00010d08)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV1_PHYS(x)                                                       ((x) + 0x00010d08)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV1_OFFS                                                          (0x00010d08)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV1_RMSK                                                                 0xf
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV1_POR                                                           0x00000000
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV1_POR_RMSK                                                      0xffffffff
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV1_ATTR                                                                 0x0
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV1_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV1_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV1_AMC_COMPLETION_IRQ_CLEAR_BMSK                                        0xf
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV1_AMC_COMPLETION_IRQ_CLEAR_SHFT                                        0x0

#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_ADDR(base,m)                                                  ((base) + 0x00010d10 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_PHYS(base,m)                                                  ((base) + 0x00010d10 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_OFFS(base,m)                                                  (0x00010d10 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_RMSK                                                              0xffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_MAXm                                                                   3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_POR                                                           0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_POR_RMSK                                                      0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_ATTR                                                                 0x3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_INI(base,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_ADDR(base,m), HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_INMI(base,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_ADDR(base,m), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_OUTI(base,m,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_ADDR(base,m),val)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_ADDR(base,m),mask,val,HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_INI(base,m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_BMSK                                            0xffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_SHFT                                               0x0

#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_ADDR(base,m)                                                            ((base) + 0x00010d14 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_PHYS(base,m)                                                            ((base) + 0x00010d14 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_OFFS(base,m)                                                            (0x00010d14 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_RMSK                                                                     0x1010000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_MAXm                                                                             3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_POR                                                                     0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_POR_RMSK                                                                0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_ATTR                                                                           0x3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_INI(base,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_ADDR(base,m), HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_INMI(base,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_ADDR(base,m), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_OUTI(base,m,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_ADDR(base,m),val)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_ADDR(base,m),mask,val,HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_INI(base,m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_AMC_MODE_TRIGGER_BMSK                                                    0x1000000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_AMC_MODE_TRIGGER_SHFT                                                         0x18
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_AMC_MODE_EN_BMSK                                                           0x10000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_AMC_MODE_EN_SHFT                                                              0x10
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_AMC_MODE_EN_AMC_MODE_DISABLE_FVAL                                              0x0
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CONTROL_AMC_MODE_EN_AMC_MODE_ENABLED_FVAL                                              0x1

#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_ADDR(base,m)                                                             ((base) + 0x00010d18 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_PHYS(base,m)                                                             ((base) + 0x00010d18 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_OFFS(base,m)                                                             (0x00010d18 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_RMSK                                                                            0x1
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_MAXm                                                                              3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_POR                                                                      0x00000001
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_POR_RMSK                                                                 0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_ATTR                                                                            0x1
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_INI(base,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_ADDR(base,m), HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_INMI(base,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_ADDR(base,m), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_CONTROLLER_IDLE_BMSK                                                            0x1
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_CONTROLLER_IDLE_SHFT                                                            0x0
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_CONTROLLER_IDLE_NOT_IDLE_FVAL                                                   0x0
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_STATUS_CONTROLLER_IDLE_IS_IDLE_FVAL                                                    0x1

#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_ADDR(base,m)                                                         ((base) + 0x00010d1c + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_PHYS(base,m)                                                         ((base) + 0x00010d1c + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_OFFS(base,m)                                                         (0x00010d1c + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_RMSK                                                                     0xffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_MAXm                                                                          3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_POR                                                                  0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_POR_RMSK                                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_ATTR                                                                        0x3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_INI(base,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_ADDR(base,m), HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_INMI(base,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_ADDR(base,m), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_OUTI(base,m,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_ADDR(base,m),val)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_ADDR(base,m),mask,val,HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_INI(base,m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_INDIVIDUAL_CMD_ENABLE_MASK_BMSK                                          0xffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV1_CMD_ENABLE_INDIVIDUAL_CMD_ENABLE_MASK_SHFT                                             0x0

#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_ADDR(base,m,n)                                                       ((base) + 0x00010d30 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_PHYS(base,m,n)                                                       ((base) + 0x00010d30 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_OFFS(base,m,n)                                                       (0x00010d30 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_RMSK                                                                    0x1010f
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_MAXm                                                                          3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_MAXn                                                                         15
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_POR                                                                  0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_POR_RMSK                                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_ATTR                                                                        0x3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_INI2(base,m,n)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_ADDR(base,m,n), HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_ADDR(base,m,n), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_OUTI2(base,m,n,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_ADDR(base,m,n),val)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_OUTMI2(base,m,n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_ADDR(base,m,n),mask,val,HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_INI2(base,m,n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_READ_OR_WRITE_BMSK                                                      0x10000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_READ_OR_WRITE_SHFT                                                         0x10
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_READ_OR_WRITE_READ_FVAL                                                     0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_READ_OR_WRITE_WRITE_FVAL                                                    0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_RES_REQ_BMSK                                                              0x100
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_RES_REQ_SHFT                                                                0x8
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_RES_REQ_FIRE_AND_FORGET_FVAL                                                0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_RES_REQ_RESPONSE_REQUIRED_FVAL                                              0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_MSG_LENGTH_BMSK                                                             0xf
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_MSGID_MSG_LENGTH_SHFT                                                             0x0

#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_ADDR(base,m,n)                                                        ((base) + 0x00010d34 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_PHYS(base,m,n)                                                        ((base) + 0x00010d34 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_OFFS(base,m,n)                                                        (0x00010d34 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_RMSK                                                                     0x7ffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_MAXm                                                                           3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_MAXn                                                                          15
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_POR                                                                   0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_POR_RMSK                                                              0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_ATTR                                                                         0x3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_INI2(base,m,n)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_ADDR(base,m,n), HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_ADDR(base,m,n), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_OUTI2(base,m,n,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_ADDR(base,m,n),val)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_OUTMI2(base,m,n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_ADDR(base,m,n),mask,val,HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_INI2(base,m,n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_SLV_ID_BMSK                                                              0x70000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_SLV_ID_SHFT                                                                 0x10
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_OFFSET_BMSK                                                               0xffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_ADDR_OFFSET_SHFT                                                                  0x0

#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_ADDR(base,m,n)                                                        ((base) + 0x00010d38 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_PHYS(base,m,n)                                                        ((base) + 0x00010d38 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_OFFS(base,m,n)                                                        (0x00010d38 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_RMSK                                                                  0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_MAXm                                                                           3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_MAXn                                                                          15
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_POR                                                                   0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_POR_RMSK                                                              0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_ATTR                                                                         0x3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_INI2(base,m,n)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_ADDR(base,m,n), HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_ADDR(base,m,n), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_OUTI2(base,m,n,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_ADDR(base,m,n),val)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_OUTMI2(base,m,n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_ADDR(base,m,n),mask,val,HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_INI2(base,m,n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_DATA_BMSK                                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_DATA_DATA_SHFT                                                                    0x0

#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_ADDR(base,m,n)                                                      ((base) + 0x00010d3c + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_PHYS(base,m,n)                                                      ((base) + 0x00010d3c + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_OFFS(base,m,n)                                                      (0x00010d3c + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_RMSK                                                                   0x10101
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_MAXm                                                                         3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_MAXn                                                                        15
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_POR                                                                 0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_POR_RMSK                                                            0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_ATTR                                                                       0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_INI2(base,m,n)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_ADDR(base,m,n), HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_ADDR(base,m,n), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_COMPLETED_BMSK                                                         0x10000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_COMPLETED_SHFT                                                            0x10
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_COMPLETED_HAS_NOT_BEEN_COMPLETE_FVAL                                       0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_COMPLETED_HAS_COMPLETED_FVAL                                               0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_ISSUED_BMSK                                                              0x100
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_ISSUED_SHFT                                                                0x8
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_ISSUED_HAS_NOT_BEEN_SENT_OVER_EPCB_FVAL                                    0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_ISSUED_HAS_BEEN_SENT_OVER_EPCB_FVAL                                        0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_TRIGGERED_BMSK                                                             0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_TRIGGERED_SHFT                                                             0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_TRIGGERED_HAS_NOT_BEEN_TRIGGERE_FVAL                                       0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_STATUS_TRIGGERED_HAS_BEEN_TRIGGERED_FVAL                                          0x1

#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_ADDR(base,m,n)                                          ((base) + 0x00010d40 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_PHYS(base,m,n)                                          ((base) + 0x00010d40 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_OFFS(base,m,n)                                          (0x00010d40 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_RMSK                                                    0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_MAXm                                                             3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_MAXn                                                            15
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_POR                                                     0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_POR_RMSK                                                0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_ATTR                                                           0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_INI2(base,m,n)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_ADDR(base,m,n), HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_ADDR(base,m,n), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_READ_RESPONSE_DATA_BMSK                                 0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_READ_RESPONSE_DATA_SHFT                                        0x0

#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_ADDR(x)                                                      ((x) + 0x00020d00)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_PHYS(x)                                                      ((x) + 0x00020d00)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_OFFS                                                         (0x00020d00)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_RMSK                                                               0xff
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_POR                                                          0x00000000
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_POR_RMSK                                                     0xffffffff
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_ATTR                                                                0x3
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_ADDR(x), HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_RMSK)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_ADDR(x),m,v,HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_IN(x))
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_AMC_COMPLETION_IRQ_ENABLE_BMSK                                     0xff
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2_AMC_COMPLETION_IRQ_ENABLE_SHFT                                      0x0

#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2_ADDR(x)                                                      ((x) + 0x00020d04)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2_PHYS(x)                                                      ((x) + 0x00020d04)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2_OFFS                                                         (0x00020d04)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2_RMSK                                                               0xff
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2_POR                                                          0x00000000
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2_POR_RMSK                                                     0xffffffff
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2_ATTR                                                                0x1
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2_IN(x)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2_ADDR(x), HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2_RMSK)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2_INM(x, m)      \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2_ADDR(x), m)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2_AMC_COMPLETION_IRQ_STATUS_BMSK                                     0xff
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2_AMC_COMPLETION_IRQ_STATUS_SHFT                                      0x0

#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV2_ADDR(x)                                                       ((x) + 0x00020d08)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV2_PHYS(x)                                                       ((x) + 0x00020d08)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV2_OFFS                                                          (0x00020d08)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV2_RMSK                                                                0xff
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV2_POR                                                           0x00000000
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV2_POR_RMSK                                                      0xffffffff
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV2_ATTR                                                                 0x0
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV2_OUT(x, v)      \
        out_dword(HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV2_ADDR(x),v)
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV2_AMC_COMPLETION_IRQ_CLEAR_BMSK                                       0xff
#define HWIO_APSS_RSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV2_AMC_COMPLETION_IRQ_CLEAR_SHFT                                        0x0

#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_ADDR(base,m)                                                  ((base) + 0x00020d10 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_PHYS(base,m)                                                  ((base) + 0x00020d10 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_OFFS(base,m)                                                  (0x00020d10 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_RMSK                                                              0xffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_MAXm                                                                   7
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_POR                                                           0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_POR_RMSK                                                      0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_ATTR                                                                 0x3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_INI(base,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_ADDR(base,m), HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_INMI(base,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_ADDR(base,m), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_OUTI(base,m,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_ADDR(base,m),val)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_ADDR(base,m),mask,val,HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_INI(base,m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_BMSK                                            0xffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_SHFT                                               0x0

#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_ADDR(base,m)                                                            ((base) + 0x00020d14 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_PHYS(base,m)                                                            ((base) + 0x00020d14 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_OFFS(base,m)                                                            (0x00020d14 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_RMSK                                                                     0x1010000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_MAXm                                                                             7
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_POR                                                                     0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_POR_RMSK                                                                0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_ATTR                                                                           0x3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_INI(base,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_ADDR(base,m), HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_INMI(base,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_ADDR(base,m), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_OUTI(base,m,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_ADDR(base,m),val)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_ADDR(base,m),mask,val,HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_INI(base,m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_AMC_MODE_TRIGGER_BMSK                                                    0x1000000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_AMC_MODE_TRIGGER_SHFT                                                         0x18
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_AMC_MODE_EN_BMSK                                                           0x10000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_AMC_MODE_EN_SHFT                                                              0x10
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_AMC_MODE_EN_AMC_MODE_DISABLE_FVAL                                              0x0
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CONTROL_AMC_MODE_EN_AMC_MODE_ENABLED_FVAL                                              0x1

#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_ADDR(base,m)                                                             ((base) + 0x00020d18 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_PHYS(base,m)                                                             ((base) + 0x00020d18 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_OFFS(base,m)                                                             (0x00020d18 + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_RMSK                                                                            0x1
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_MAXm                                                                              7
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_POR                                                                      0x00000001
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_POR_RMSK                                                                 0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_ATTR                                                                            0x1
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_INI(base,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_ADDR(base,m), HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_INMI(base,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_ADDR(base,m), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_CONTROLLER_IDLE_BMSK                                                            0x1
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_CONTROLLER_IDLE_SHFT                                                            0x0
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_CONTROLLER_IDLE_NOT_IDLE_FVAL                                                   0x0
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_STATUS_CONTROLLER_IDLE_IS_IDLE_FVAL                                                    0x1

#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_ADDR(base,m)                                                         ((base) + 0x00020d1c + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_PHYS(base,m)                                                         ((base) + 0x00020d1c + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_OFFS(base,m)                                                         (0x00020d1c + 0x2A0 * (m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_RMSK                                                                     0xffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_MAXm                                                                          7
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_POR                                                                  0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_POR_RMSK                                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_ATTR                                                                        0x3
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_INI(base,m)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_ADDR(base,m), HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_INMI(base,m,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_ADDR(base,m), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_OUTI(base,m,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_ADDR(base,m),val)
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_ADDR(base,m),mask,val,HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_INI(base,m))
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_INDIVIDUAL_CMD_ENABLE_MASK_BMSK                                          0xffff
#define HWIO_APSS_RSC_RSCC_TCSm_DRV2_CMD_ENABLE_INDIVIDUAL_CMD_ENABLE_MASK_SHFT                                             0x0

#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_ADDR(base,m,n)                                                       ((base) + 0x00020d30 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_PHYS(base,m,n)                                                       ((base) + 0x00020d30 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_OFFS(base,m,n)                                                       (0x00020d30 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_RMSK                                                                    0x1010f
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_MAXm                                                                          7
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_MAXn                                                                         15
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_POR                                                                  0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_POR_RMSK                                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_ATTR                                                                        0x3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_INI2(base,m,n)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_ADDR(base,m,n), HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_ADDR(base,m,n), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_OUTI2(base,m,n,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_ADDR(base,m,n),val)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_OUTMI2(base,m,n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_ADDR(base,m,n),mask,val,HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_INI2(base,m,n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_READ_OR_WRITE_BMSK                                                      0x10000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_READ_OR_WRITE_SHFT                                                         0x10
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_READ_OR_WRITE_READ_FVAL                                                     0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_READ_OR_WRITE_WRITE_FVAL                                                    0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_RES_REQ_BMSK                                                              0x100
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_RES_REQ_SHFT                                                                0x8
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_RES_REQ_FIRE_AND_FORGET_FVAL                                                0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_RES_REQ_RESPONSE_REQUIRED_FVAL                                              0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_MSG_LENGTH_BMSK                                                             0xf
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_MSGID_MSG_LENGTH_SHFT                                                             0x0

#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_ADDR(base,m,n)                                                        ((base) + 0x00020d34 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_PHYS(base,m,n)                                                        ((base) + 0x00020d34 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_OFFS(base,m,n)                                                        (0x00020d34 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_RMSK                                                                     0x7ffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_MAXm                                                                           7
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_MAXn                                                                          15
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_POR                                                                   0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_POR_RMSK                                                              0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_ATTR                                                                         0x3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_INI2(base,m,n)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_ADDR(base,m,n), HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_ADDR(base,m,n), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_OUTI2(base,m,n,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_ADDR(base,m,n),val)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_OUTMI2(base,m,n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_ADDR(base,m,n),mask,val,HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_INI2(base,m,n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_SLV_ID_BMSK                                                              0x70000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_SLV_ID_SHFT                                                                 0x10
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_OFFSET_BMSK                                                               0xffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_ADDR_OFFSET_SHFT                                                                  0x0

#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_ADDR(base,m,n)                                                        ((base) + 0x00020d38 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_PHYS(base,m,n)                                                        ((base) + 0x00020d38 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_OFFS(base,m,n)                                                        (0x00020d38 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_RMSK                                                                  0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_MAXm                                                                           7
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_MAXn                                                                          15
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_POR                                                                   0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_POR_RMSK                                                              0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_ATTR                                                                         0x3
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_INI2(base,m,n)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_ADDR(base,m,n), HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_ADDR(base,m,n), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_OUTI2(base,m,n,val)    \
        out_dword(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_ADDR(base,m,n),val)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_OUTMI2(base,m,n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_ADDR(base,m,n),mask,val,HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_INI2(base,m,n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_DATA_BMSK                                                             0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_DATA_DATA_SHFT                                                                    0x0

#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_ADDR(base,m,n)                                                      ((base) + 0x00020d3c + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_PHYS(base,m,n)                                                      ((base) + 0x00020d3c + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_OFFS(base,m,n)                                                      (0x00020d3c + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_RMSK                                                                   0x10101
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_MAXm                                                                         7
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_MAXn                                                                        15
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_POR                                                                 0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_POR_RMSK                                                            0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_ATTR                                                                       0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_INI2(base,m,n)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_ADDR(base,m,n), HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_ADDR(base,m,n), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_COMPLETED_BMSK                                                         0x10000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_COMPLETED_SHFT                                                            0x10
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_COMPLETED_HAS_NOT_BEEN_COMPLETE_FVAL                                       0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_COMPLETED_HAS_COMPLETED_FVAL                                               0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_ISSUED_BMSK                                                              0x100
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_ISSUED_SHFT                                                                0x8
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_ISSUED_HAS_NOT_BEEN_SENT_OVER_EPCB_FVAL                                    0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_ISSUED_HAS_BEEN_SENT_OVER_EPCB_FVAL                                        0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_TRIGGERED_BMSK                                                             0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_TRIGGERED_SHFT                                                             0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_TRIGGERED_HAS_NOT_BEEN_TRIGGERE_FVAL                                       0x0
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_STATUS_TRIGGERED_HAS_BEEN_TRIGGERED_FVAL                                          0x1

#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_ADDR(base,m,n)                                          ((base) + 0x00020d40 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_PHYS(base,m,n)                                          ((base) + 0x00020d40 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_OFFS(base,m,n)                                          (0x00020d40 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_RMSK                                                    0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_MAXm                                                             7
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_MAXn                                                            15
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_POR                                                     0x00000000
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_POR_RMSK                                                0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_ATTR                                                           0x1
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_INI2(base,m,n)        \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_ADDR(base,m,n), HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_RMSK)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_ADDR(base,m,n), mask)
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_READ_RESPONSE_DATA_BMSK                                 0xffffffff
#define HWIO_APSS_RSC_RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA_READ_RESPONSE_DATA_SHFT                                        0x0


#endif /* __RSC_HAL_HWIO_H__ */
